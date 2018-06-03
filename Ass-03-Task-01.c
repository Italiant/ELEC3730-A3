// This is the console task. Can use your code from the previous assignment
// and expand.
//
// The example code is very basic, showing access to the SD card. Note that
// if the SD card is access from both this task and the pulse rate monitor
// task, a MUTEX will be required.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
//--------------------- Includes ---------------------
#include "Ass-03.h"
#include <stdlib.h>

// --------------------- Function Headers ---------------------
//uint8_t myReadFile();
//uint8_t myWriteFile();
int string_parser(uint8_t *inp, uint8_t **array_of_words_p[]);
int debug(int *debug);
int analog_f(int *analog, uint8_t** string);
int ls_f();
FRESULT scan_files(char* path);

// --------------------- File & Global Variables ---------------------
FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;
int analog_global = 10;
int flag = 0;
int debug_global;

// --------------------- Task 1: Main ---------------------
void Ass_03_Task_01(void const * argument)
{
	safe_printf("Hello from Task 1 - Console (serial input)\n");
	safe_printf("INFO: Initialise LCD and TP first...\n");

	// Initialize and turn on LCD, calibrate the touch panel and change colour to white
	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_TP_Init();
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	// Signal other tasks to start
	osSignalSet(myTask02Handle, 1);
	osSignalSet(myTask03Handle, 1);
	osSignalSet(myTask04Handle, 1);

	// Declaring variables
	uint8_t **strs; // Array to store the input string divided up into arguments
	uint8_t c;
	uint8_t* input1 = (uint8_t*)malloc(sizeof(uint8_t)*20); // Array to store the input characters
	int pos = 0;
	int words = 0;
	int debug1 = 0;
	int analog = 10;

	// Must stay in while loop forever for synchronous tasks to work together
	while (1)
	{
		debug1 = debug_global; // Synchronize local debug to global one if changed

		// Get input from console via getchar();
		safe_printf(">");
		c = getchar();
		input1[pos] = c;

		// If enter key is pressed
		if(input1[pos] == '\r'){ // \r is \n in putty
			input1[pos] = '\0'; // Set last position to NULL
			safe_printf("Input String = %s\n", input1); // Print out inputted string
			words = string_parser(input1, &strs); // Send input character array to string parser function

			// Display words entered if debug is pressed
			if(debug){
				safe_printf("> %s\n", input1);
				safe_printf("Count	: %d\n", words);
				for(int j = 0; j < words; j++){ // prints out words
					safe_printf("Word(%d)	: %s\n", j, strs[j]);
				}
			}
			pos = 0; // Resets position for next character string
			
			// Argument checks
			// Debug -> Turns debugging on or off
			if(strcmp((const char *)strs[0], "debug") == 0){
				debug(&debug1);
				debug_global = debug1;
				//safe_printf("Debug is %d\n", debug1);
			}
			// Analog <time> -> Changes scale of plotting graph
			else if(strcmp((const char *)strs[0], "analog") == 0){
				analog_f(&analog, strs);
				analog_global = analog;
				osMessagePut (myQueue03Handle, analog, 0);
				flag = 1;
			}
			ls -> List contents of current directory folder
			else if(strcmp((const char *)strs[0], "ls") == 0){
				ls_f();
			}
			//myReadFile();
			//myWriteFile();
			
		// Else if enter key is not pressed
		}else{
			safe_printf("%c", input1[pos]);
			pos++; // Increace position
		}
	}

}

// --------------------- Functions ---------------------

// Function: List Directory Contents
// Input: 
// Result: 
int ls_f(){
	FATFS fs;
	FRESULT res;
	char buff[256];
		res = f_mount(&fs, "", 1);
		if (res == FR_OK) {
			strcpy(buff, "/");
			res = scan_files(buff);
		}
	return 0;
}

// Function: Scan Files
// Input: A valid char pointer to the directory path
// Result: Lists the contents of the specified path
FRESULT scan_files(char* path){
	FRESULT res;
	DIR dir;
	int i;
	static FILINFO inf;
	int folders = 0, files = 0;

	res = f_opendir(&dir, path);                       /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &inf);                   /* Read a directory item */
			if (res != FR_OK || inf.fname[0] == 0) break;  /* Break on error or end of dir */
			if (inf.fattrib & AM_DIR) {                    /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", inf.fname);
				res = scan_files(path);                    /* Enter the directory */
				if (res != FR_OK) break;
				path[i] = 0;
				safe_printf("%s (folder)\n", inf.fname);
				folders++;
			} else {                                       /* It is a file. */
				safe_printf("%s (%d bytes)\n", inf.fname, inf.fsize);
				//printf("%s\n",inf.fattrib);
				//safe_printf("Date: %d\n", inf.fdate);
				//safe_printf("Time: %d\n", inf.ftime);
				files++;
			}
		}
		f_closedir(&dir);
	}
	safe_printf("Folders: %d\n", folders);
	safe_printf("Files: %d\n", files);
	return res;
}

// Function: Plot Analog Input
// Input: 
// Result: 
int analog_f(int *analog, uint8_t** string){
	int number;
	char *pnt;
	int pre;
	int error = 0;

	if(string[1] == '\0'){

	}else{
			strtod((const char *)string[1], &pnt);
			if(*pnt != 0){
				safe_printf("ERROR: Number conversion unsuccessful\n");
				error = 1;
			}
			if(*analog < 0){
				safe_printf("ERROR: Number must be positive\n");
				error = 1;
			}
			else if (error == 0){
				number = atoi((const char *)string[1]);
				pre = *analog;
				*analog = number;
				safe_printf("Plotting time changed from (%d)s to (%d)s\n", pre, *analog);
			}
	}
	return 0;
}

// Function: Debug
// Input: 
// Result: 
int debug(int *debug){
	int ret;
		*debug = !*debug;
		if(*debug){
			ret = 1;
			safe_printf("Debug messages will be displayed\n");
		}else{
			safe_printf("Debug messages will not be displayed\n");
			ret = 0;
		}
	return ret;
}

// Function: String Parser
// Input: 
// Result: 
int string_parser(uint8_t *inp, uint8_t **array_of_words_p[]){

	int string_length = 0;
	for (int i = 0; inp[i] != '\0'; i++){		// reading though getting the length of the input string
		string_length++;
	}

	uint8_t *pinp = (uint8_t*)malloc(sizeof(uint8_t) * string_length);
	int pinp_it = 0;
	for (int i = 0; inp[i] != '\0'; i++){
		if (inp[i] != 8){
			pinp[pinp_it] = inp[i];
			pinp_it++;
		}
		else if (pinp_it > 0){
			pinp_it--;
		}
	}
	pinp[pinp_it] = '\0';

	int letter_count = 0;
	int word_count = 0;
	for (int i = 0; inp[i] != '\0'; i++){
		if ((i > 0 && inp[i] == ' ' && inp[i-1] != ' ') 	// if the character is a space and the previous (reason for i > 0) character wasnt a space
				|| (inp[i+1] == '\0' && inp[i] != ' ')){		// OR this is the last word and the character isnt a space i.e. it is a word
			word_count++;
		}
	}

	*array_of_words_p = (uint8_t**)malloc(sizeof(uint8_t*) * (word_count));
	word_count = 0;

	for (int i = 0; pinp[i] != '\0'; i++){
		if ((i > 0 && pinp[i] == ' ' && pinp[i-1] != ' ') || (pinp[i+1] == '\0' && pinp[i] != ' ')){	// same word check as before
			if (pinp[i+1] == '\0'){	//if this is a word and it is the last word
				letter_count++;		// off by one error says i need to increment the letter count...
			}
			(*array_of_words_p)[word_count] = (uint8_t*)malloc(sizeof(uint8_t)*(letter_count+1));		//allocate memory of lettercount size at this words point in the array

			letter_count = 0;		//reset lettercount for the next word
			word_count++;
		}
		if (i > 0 && pinp[i] == ' ' && pinp[i-1] == ' '){		// if this is a space and the previous character was a space
			continue;										// dont increment the letter count just continue reading
		}
		else {
			letter_count++;
		}
	}

	word_count = 0;
	letter_count = 0;

	for (int i = 0; pinp[i] != '\0'; i++){
		if ((i > 0 && pinp[i] == ' ' && pinp[i-1] != ' ') || (pinp[i+1] == '\0' && pinp[i] != ' ')){	// same word check again
			if (pinp[i+1] == '\0'){	//if this is a word and it is the last word
				(*array_of_words_p)[word_count][letter_count] = pinp[i];
				letter_count++;
			}
			(*array_of_words_p)[word_count][letter_count] = '\0';
			word_count++;
			letter_count = 0;
		}
		else if (pinp[i] == ' '){		// if this is a space and the previous character was a space
			continue;										// dont increment the letter count just continue reading
		}
		else{
			(*array_of_words_p)[word_count][letter_count] = pinp[i];
			letter_count++;
		}
	}
	free(pinp);

	return word_count;

}

/*
// Function: Read File
// Input: 
// Result: 
uint8_t myReadFile()
{
#define READ_FILE "Hello.txt"
#define BUFF_SIZE 256
	uint8_t rtext[BUFF_SIZE];
	FRESULT res;
	uint32_t bytesread;

	// Open file Hello.txt
	if((res = f_open(&MyFile, READ_FILE, FA_READ)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", READ_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", READ_FILE);

	// Read data from file
	if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
	{
		safe_printf("ERROR: Reading '%s'\n", READ_FILE);
		f_close(&MyFile);
		return 1;
	}
	rtext[bytesread] = '\0';
	safe_printf("Task 1: Read: '%s'\n", rtext);

	// Close file
	f_close(&MyFile);

	return 0;
}

// Function: Write File
// Input: 
// Result: 
uint8_t myWriteFile()
{
#define WRITE_FILE "There.txt"
	FRESULT res;
	UINT byteswritten;

	// Open file There.txt
	if((res = f_open(&MyFile, WRITE_FILE, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", WRITE_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", WRITE_FILE);

	// Write to file
	if ((res = f_write(&MyFile, "Hello", 6, &byteswritten)) != FR_OK)
	{
		safe_printf("ERROR: Writing '%s'\n", WRITE_FILE);
		f_close(&MyFile);
		return 1;
	}
	safe_printf("Task 1: Written: %d bytes\n", byteswritten);

	// Close file
	f_close(&MyFile);

	return 0;
}
*/
