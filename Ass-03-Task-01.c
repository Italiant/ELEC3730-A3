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

// --------------------- Function Headers ---------------------
//uint8_t myReadFile();
//uint8_t myWriteFile();
int string_parser(uint8_t *input_p, uint8_t **array_of_words_p[]);
int debug(int *debug);
int analog_f(int *analog, uint8_t** string, int debug2);
int ls_f();
FRESULT scan_files(char* path);
void helpfn(uint8_t** string);
int8_t cd_f(uint8_t** string, uint8_t word_count);
int8_t mkdir_f(uint8_t** string);
int8_t rm_f(uint8_t** string);
int8_t cp_f(uint8_t** string);

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
	osEvent event1;

	//myReadFile();
	//myWriteFile();

	while (1)// Must stay in while loop forever for synchronous tasks to work together
	{
		debug1 = debug_global; // Synchronize local debug to global debug

		// Get input from console via getchar();
		c = getchar();
		input1[pos] = c;

		// If enter key is pressed
		if(input1[pos] == '\r'){ // \r is \n in putty
			input1[pos] = '\0'; // Set last position to NULL
			if(debug1){
				safe_printf("\nInput String = %s\n", input1); // Print out inputted string
			}
			words = string_parser(input1, &strs); // Send input character array to string parser function

			// Display words entered if debug is pressed
			if(debug1){
				safe_printf("> %s\n", input1);
				safe_printf("Count	: %d\n", words);
				for(int j = 0; j < words; j++){ // prints out words
					safe_printf("Word(%d)	: %s\n", j, strs[j]);
				}
			}
			pos = 0; // Resets position for next character string

			// Argument checks
			// Debug -> Turns debugging on or off
			if((strcmp((const char *)strs[0], "debug") == 0) && words == 1){
				debug(&debug1);
				debug_global = debug1;
				//safe_printf("Debug is %d\n", debug1);
			}
			// Analog <time> -> Changes scale of plotting graph
			else if((strcmp((const char *)strs[0], "analog") == 0) && words > 1){
				analog_f(&analog, strs, debug1);
				osMessagePut (myQueue03Handle, analog, 0);
			}
			// Help <command> -> Prints useful help messages
			else if((strcmp((const char *)strs[0], "help") == 0) && words > 1){
				helpfn(strs);
			}
			// ls -> List contents of current directory folder
			else if((strcmp((const char *)strs[0], "ls") == 0) && words == 1){
				ls_f();
			}
			// cd <dir> -> Changes current working directory
			else if((strcmp((const char *)strs[0], "cd") == 0) && words > 1){
				cd_f(strs, words);
			}
			// mkdir <dir> -> Creates a new folder
			else if((strcmp((const char *)strs[0], "mkdir") == 0) && words > 1){
				mkdir_f(strs);
			}
			// rm <file> -> deletes the file
			else if((strcmp((const char *)strs[0], "rm") == 0) && words > 1){
				rm_f(strs);
			}
			// cp <src> <dst> -> copies the file to the destination
			else if((strcmp((const char *)strs[0], "cp") == 0) && words > 1){
				cp_f(strs);
			}
			else{
				safe_printf("'%s' is an invalid argument, try:\n", strs[0]);
				safe_printf("\t debug\n \t ls\n \t analog <time>\n \t cd <dir>\n \t mkdir <dir>\n \t rm <file>\n \t cp <src> <dst>\n \t help <command>\n");
			}

		}else{// Else if enter key is not pressed
			safe_printf("Got(%c)\n", c);
			pos++; // Increase position
		}
	}

}

// --------------------- Functions ---------------------

// Function: Help
// Input: A valid array of words
// Output: Prints useful help messages for the input command
void helpfn(uint8_t** string){

	if((strcmp((const char *)string[1], "debug") == 0)){

		safe_printf("debug : toggles between debug on or off\n");
	}

	else if((strcmp((const char *)string[1], "analog") == 0)){

		safe_printf("analog <time> : changes the plot of the analog input for the given time period <time>\n");
	}

	else if((strcmp((const char *)string[1], "ls") == 0)){

		safe_printf("ls : lists the contents of the current folder\n");
	}

	else if((strcmp((const char *)string[1], "cd") == 0)){

		safe_printf("cd <dir> : changes the current working folder to <dir>\n");
	}

	else if((strcmp((const char *)string[1], "mkdir") == 0)){

		safe_printf("mkdir <dir> : creates a new folder <dir>\n");
	}

	else if((strcmp((const char *)string[1], "rm") == 0)){

		safe_printf("rm <file> : deletes the file <file>\n");
	}

	else if((strcmp((const char *)string[1], "cp") == 0)){

		safe_printf("cp <src> <dst> : copies the file <src> to the destination <dst>\n");
	}

	else{
		safe_printf("ERROR: Unknown help command '%s', try help <command>:\n", (*string)[1]);
		safe_printf("\t debug\n \t ls\n \t analog\n \t cd\n \t mkdir\n \t rm\n \t cp\n");
	}
}

// Function: Copy File
// Input: A valid array of words
// Output: Copies the specified file to the destination location
int8_t cp_f(uint8_t** string){
	FRESULT res;
	res = f_rename((*string)[1], (*string)[2]);
	if (res != FR_OK){
		safe_printf("ERROR: Could not copy file '%s' to '%s'", (*string)[1], (*string)[2]);
		return 1;
	}else{
		safe_printf("The file '%s' was copied to '%s'", (*string)[1], (*string)[2]);
		return 0;
	}
	return 0;
}

// Function: Delete File
// Input: A valid array of words
// Output: Deletes the specified file
int8_t rm_f(uint8_t** string){
	FILINFO * info;
	FRESULT res;
	res = f_stat((*string)[1], info);
	if (res == FR_INVALID_NAME){
		safe_printf("ERROR: The file '%s' does not exist\n", (*string)[1]);
		return 1;

	}else{
		res = f_unlink((*string)[1]);
		if (res==FR_OK){
			safe_printf("File '%s' removed\n", (*string)[1]);
		}else if(res == FR_DENIED){
			printf("ERROR: Could not remove file '%s', directory must be empty\n", (*string)[1]);
		}else{
			safe_printf("ERROR: Unknown file command '%d'\n", res);
		}
	}
	return 0;
}

//Function: Change Current Directory
// Input: A valid array of words and the number of words
// Output: Changes the current working folder
int8_t cd_f(uint8_t** string, uint8_t word_count){
	FRESULT res;
	if (word_count < 3) {
		if (word_count < 2) {
			(*string)[1] = "/";
		}
		// change directory to the path
		res = f_chdir((*string)[1]);
		if (res != FR_OK) {
			safe_printf("ERROR: Unknown directory command '%d'\n", res);
			return 1;
		}else{
			safe_printf("Directory changed to '%s'\n", (*string)[1]);
		}
	}
	else {
		safe_printf("ERROR: Incorrect argument\n");
	}

	return 0;
}

// Function: Make a New Directory
// Input: A valid array of words
// Output: Creates a new folder
int8_t mkdir_f(uint8_t** string){
	FRESULT res;
	res = f_mkdir((*string)[1]);
	if (res != FR_OK) {
		safe_printf("ERROR: Unknown directory command '%d'\n", res);
		return 1;
	}else{
		safe_printf("Folder '%s' created\n", (*string)[1]);
	}

	return 0;
}

// Function: List Directory Contents
// Input: n/a
// Result: Prints the contents of the current folder
int ls_f(){
	FATFS fs;
	FRESULT res;
	DIR dir;
	char* path;
	int i;
	static FILINFO inf;
	int folders = 0, files = 0;
	UINT pathlength = 20;

	res = f_getcwd(path, pathlength);
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
				folders += 1;
			} else {                                       /* It is a file. */
				safe_printf("%s (%d bytes)\n", inf.fname, inf.fsize);
				//printf("%s\n",inf.fattrib);
				//safe_printf("Date: %d\n", inf.fdate);
				//safe_printf("Time: %d\n", inf.ftime);
				files += 1;
				break;
			}
		}
		safe_printf("Folders: %d\n", folders);
		safe_printf("Files: %d\n", files);
		f_closedir(&dir);
	}else{
		safe_printf("ERROR: SD card could not be read properly\n");
		return 0;
	}
	return res;
}

//	char buff[256];
//	res = f_mount(&fs, "", 1);
//	if (res == FR_OK) {
//		strcpy(buff, "/");
//		res = scan_files(buff);
//	}
//	return 0;
//}

// Function: Scan Files
// Input: A valid char pointer to the directory path
// Result: Lists the contents of the specified path
//FRESULT scan_files(char* path){
//	FRESULT res;
//	DIR dir;
//	int i;
//	static FILINFO inf;
//	int folders = 0, files = 0;
//
//	res = f_opendir(&dir, path);                       /* Open the directory */
//	if (res == FR_OK) {
//		for (;;) {
//			res = f_readdir(&dir, &inf);                   /* Read a directory item */
//			if (res != FR_OK || inf.fname[0] == 0) break;  /* Break on error or end of dir */
//			if (inf.fattrib & AM_DIR) {                    /* It is a directory */
//				i = strlen(path);
//				sprintf(&path[i], "/%s", inf.fname);
//				res = scan_files(path);                    /* Enter the directory */
//				if (res != FR_OK) break;
//				path[i] = 0;
//				safe_printf("%s (folder)\n", inf.fname);
//				folders++;
//			} else {                                       /* It is a file. */
//				safe_printf("%s (%d bytes)\n", inf.fname, inf.fsize);
//				//printf("%s\n",inf.fattrib);
//				//safe_printf("Date: %d\n", inf.fdate);
//				//safe_printf("Time: %d\n", inf.ftime);
//				files++;
//			}
//		}
//		f_closedir(&dir);
//	}
//	safe_printf("Folders: %d\n", folders);
//	safe_printf("Files: %d\n", files);
//	return res;
//}

// Function: Plot Analog Input
// Input: A valid pointer to the analog variable, the input string and the debug option
// Result: Updates and converts analog value to an integer and updates scale on LCD screen
int analog_f(int *analog, uint8_t** string, int debug2){
	int number;
	char *pnt;
	int pre;
	int error = 0;
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
		if(debug2){
			safe_printf("Plotting time changed from (%d)s to (%d)s\n", pre, *analog);
		}
		osMutexWait(myMutex01Handle, osWaitForever);

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillRect(225, 195, 93, 12);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_SetFont(&Font12);
		uint8_t buffer[10]; // Buffer string
		sprintf((char *)buffer, "%d", *analog);
		BSP_LCD_DisplayStringAt(306, 195, buffer, RIGHT_MODE); // 10s: (133,4)>(318,192)
		BSP_LCD_DisplayStringAt(307, 195, (uint8_t*)"s", LEFT_MODE); // 10s: (133,4)>(318,192)

		osMutexRelease(myMutex01Handle);
	}
	return 0;
}

// Function: Debug
// Input: A valid pointer to an integer
// Result: Toggles debug on or off
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
// Input: A valid input string and a pointer to a double array which will be the new array of words
// Result: Updates array of words by seperating the input string into words
int string_parser(uint8_t *input_p, uint8_t **array_of_words_p[]){

	int string_length = 0;
	for (int i = 0; input_p[i] != '\0'; i++){		// reading though getting the length of the input string
		string_length++;
	}

	uint8_t *temp = (uint8_t*)malloc(sizeof(uint8_t) * string_length);
	int temp_pos = 0;
	for (int i = 0; input_p[i] != '\0'; i++){
		if (input_p[i] != 8){
			temp[temp_pos] = temp[i];
			temp_pos++;
		}
		else if (temp_pos > 0){
			temp_pos--;
		}
	}
	temp[temp_pos] = '\0';

	int letter_count = 0;
	int word_count = 0;
	for (int i = 0; input_p[i] != '\0'; i++){
		if ((i > 0 && input_p[i] == ' ' && input_p[i-1] != ' ') 	// if the character is a space and the previous (reason for i > 0) character wasnt a space
				|| (input_p[i+1] == '\0' && input_p[i] != ' ')){		// OR this is the last word and the character isnt a space i.e. it is a word
			word_count++;
		}
	}

	*array_of_words_p = (uint8_t**)malloc(sizeof(uint8_t*) * (word_count));
	word_count = 0;

	for (int i = 0; temp[i] != '\0'; i++){
		if ((i > 0 && temp[i] == ' ' && temp[i-1] != ' ') || (temp[i+1] == '\0' && temp[i] != ' ')){	// same word check as before
			if (temp[i+1] == '\0'){	//if this is a word and it is the last word
				letter_count++;		// off by one error says i need to increment the letter count...
			}
			(*array_of_words_p)[word_count] = (uint8_t*)malloc(sizeof(uint8_t)*(letter_count+1));		//allocate memory of lettercount size at this words point in the array

			letter_count = 0;		//reset lettercount for the next word
			word_count++;
		}
		if (i > 0 && temp[i] == ' ' && temp[i-1] == ' '){		// if this is a space and the previous character was a space
			continue;										// dont increment the letter count just continue reading
		}
		else {
			letter_count++;
		}
	}

	word_count = 0;
	letter_count = 0;

	for (int i = 0; temp[i] != '\0'; i++){
		if ((i > 0 && temp[i] == ' ' && temp[i-1] != ' ') || (temp[i+1] == '\0' && temp[i] != ' ')){	// same word check again
			if (temp[i+1] == '\0'){	//if this is a word and it is the last word
				(*array_of_words_p)[word_count][letter_count] = temp[i];
				letter_count++;
			}
			(*array_of_words_p)[word_count][letter_count] = '\0';
			word_count++;
			letter_count = 0;
		}
		else if (temp[i] == ' '){		// if this is a space and the previous character was a space
			continue;										// dont increment the letter count just continue reading
		}
		else{
			(*array_of_words_p)[word_count][letter_count] = temp[i];
			letter_count++;
		}
	}
	free(temp);

	return word_count;

}


//// Function: Read File
//// Input: 
//// Result: 
//uint8_t myReadFile()
//{
//#define READ_FILE "Hello.txt"
//#define BUFF_SIZE 256
//	uint8_t rtext[BUFF_SIZE];
//	FRESULT res;
//	uint32_t bytesread;
//
//	// Open file Hello.txt
//	if((res = f_open(&MyFile, READ_FILE, FA_READ)) != FR_OK)
//	{
//		safe_printf("ERROR: Opening '%s'\n", READ_FILE);
//		return 1;
//	}
//	safe_printf("Task 1: Opened file '%s'\n", READ_FILE);
//
//	// Read data from file
//	if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
//	{
//		safe_printf("ERROR: Reading '%s'\n", READ_FILE);
//		f_close(&MyFile);
//		return 1;
//	}
//	rtext[bytesread] = '\0';
//	safe_printf("Task 1: Read: '%s'\n", rtext);
//
//	// Close file
//	f_close(&MyFile);
//
//	return 0;
//}
//
//// Function: Write File
//// Input: 
//// Result: 
//uint8_t myWriteFile()
//{
//#define WRITE_FILE "Hello.txt"
//	FRESULT res;
//	UINT byteswritten;
//
//	// Open file There.txt
//	if((res = f_open(&MyFile, WRITE_FILE, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
//	{
//		safe_printf("ERROR: Opening '%s'\n", WRITE_FILE);
//		return 1;
//	}
//	safe_printf("Task 1: Opened file '%s'\n", WRITE_FILE);
//
//	// Write to file
//	if ((res = f_write(&MyFile, "Hello", 6, &byteswritten)) != FR_OK)
//	{
//		safe_printf("ERROR: Writing '%s'\n", WRITE_FILE);
//		f_close(&MyFile);
//		return 1;
//	}
//	safe_printf("Task 1: Written: %d bytes\n", byteswritten);
//
//	// Close file
//	f_close(&MyFile);
//
//	return 0;
//}
