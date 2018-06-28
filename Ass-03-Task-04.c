// This is the task that reads the analog input. A buffer is divided in two to
// allow working on one half of the buffer while the other half is being
// loaded using the DMA controller.
//
// This task also plots the wave form to the screen.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Note that there needs to be a way of starting and stopping the display.

//--------------------- Includes ---------------------
#include "Ass-03.h"

//--------------------- Function Headers ---------------------
uint8_t write_file(uint16_t *data, uint32_t M);
uint8_t read_file(uint32_t M);

//--------------------- Defines ---------------------
// Defines coordinates for graph region
#define XOFF 134
#define YOFF 5
#define XSIZE 182
#define YSIZE 187

//--------------------- Global Variables & ADC ---------------------
int debug_global;
uint16_t ADC_Value[1000];

//--------------------- Task 4: Main ---------------------
void Ass_03_Task_04(void const * argument)
{
	// Declare variables
	uint16_t i;
	HAL_StatusTypeDef status;
	uint16_t xpos=0;
	uint16_t ypos=0;
	uint16_t last_xpos=0;
	uint16_t last_ypos=0;
	uint32_t start = 1;
	uint32_t analog = 10;
	uint8_t memory = 0;

	// Event variables
	osEvent event1, event2, event3, event4;
	
	// Used to mesh data together
	int j = 0;
	int k = 0;
	
	// Used to hold meshed data
	uint16_t data_train[182];
	for(k = 0; k < XSIZE; k++){											// Initializes data array with 98 which in y position terms is a straight line
		data_train[k] = 98;
	}
	// Waits for signal from task 1 to start
	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

	// Start the conversion process
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 1000);
	if (status != HAL_OK)
	{
		safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
	}

	// Start main loop
	while (1)
	{
		// Wait for message from task 2 to start and stop plotting the graph
		event1 = osMessageGet(myQueue02Handle, 5); 						// Waits 5ms each loop to receive the message and trigger the small interrupt 
		if (event1.status == osEventMessage) 							// Checks if the received message has any information in it
		{
			start = event1.value.v;										// Saves the event value from the message to the local variable
			if(start){
				safe_printf("Plotting graph\n");
			}else{
				safe_printf("Stopped plotting graph\n");
			}
		}

		// Wait for message from task 2 to receive analog input
		event2 = osMessageGet(myQueue03Handle, 5);
		if (event2.status == osEventMessage)
		{
			analog = event2.value.v;

			safe_printf("Plotting time changed to (%d)s\n", analog);
		}

		// Wait for message from task 2 to receive memory position - STORE
		event3 = osMessageGet(myQueue04Handle, 5);
		if (event3.status == osEventMessage)
		{
			memory = event3.value.v;

			if(debug_global){
				safe_printf("File memory (%d) selected\n", memory);
			}
			write_file(data_train, memory);								// Calls write function and sends the data array with x and y positions and the memory position
		}

		// Wait for message from task 2 to receive memory position - LOAD
		event4 = osMessageGet(myQueue05Handle, 5);
		if (event4.status == osEventMessage)
		{
			memory = event4.value.v;

			if(debug_global){
				safe_printf("File memory (%d) selected\n", memory);
			}					
			read_file(memory);											// Calls the read function and sends the memory position
		}

		if(start){ // Used to start and stop plotting the graph

			// Wait for first half of buffer
			osSemaphoreWait(myBinarySem05Handle, 1000/(18.2/(analog/10)));	// Changed semaphore so it acts as an analog timer which adjusts the time scale of the plotting graph
			//osSemaphoreWait(myBinarySem05Handle, osWaitForever);			// Changed from this line
			osMutexWait(myMutex01Handle, osWaitForever);
			for(i=0;i<500;i=i+500)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				ypos=(uint16_t)((uint32_t)(ADC_Value[i])*YSIZE/4096);
				BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
				last_xpos=xpos;
				last_ypos=ypos;
				xpos += 1;
			}
			osMutexRelease(myMutex01Handle);

			data_train[j] = ypos; 										// Stores the y position in the data train array for each corresponding x position for the first half of the buffer
			j++;														// Increments j which is the x position

			if (last_xpos>=XSIZE-1)
			{
				xpos=0;
				last_xpos=0;
			}

			// Wait for second half of buffer
			osSemaphoreWait(myBinarySem06Handle, 1000/(18.2/(analog/10)));	// Also changed this semaphore as well to make it into an analog timer
			//osSemaphoreWait(myBinarySem06Handle, osWaitForever);
			HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
			osMutexWait(myMutex01Handle, osWaitForever);
			for(i=0;i<500;i=i+500)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				ypos=(uint16_t)((uint32_t)(ADC_Value[i+500])*YSIZE/4096);
				BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
				// BSP_LCD_FillCircle(xpos,ypos,2);
				last_xpos=xpos;
				last_ypos=ypos;
				//data[xpos] = ypos;
				xpos += 1;
			}
			osMutexRelease(myMutex01Handle);

			data_train[j] = ypos;										// Stores the y position in the data train array for each corresponding x position for the second half of the buffer
			j++;														// Increments j which is the x position

			if (last_xpos>=XSIZE-1)
			{
				xpos=0;
				last_xpos=0;
			}


			HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);
		}
		else{
			// If start is == 0 then do not plot the graph, pause it
		}
		if(j >= 182){													// When the x position has reached the end of the plotting screen (or greater because semaphores miss the check if it is only one point)
			j = 0;														// Reset the x position to 0
			for(k = 0; k < XSIZE; k++){									// Resets the data array to all be 98 which is exactly half of YSIZE which becomes a straight line
				data_train[k] = 98;
			}
		}
	}
}

// ----------------------------------------- Functions -----------------------------------------

// Add callback functions to see if this can be used for double buffering equivalent

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	//osSemaphoreRelease(myBinarySem05Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
	//osSemaphoreRelease(myBinarySem06Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
}

// Function: Write File
// Input: A valid pointer to the data array holding the x and y positions and a memory position
// Output: Writes the data to the SD card memory position
uint8_t write_file(uint16_t *data, uint32_t M)
{
	FRESULT res;																		// Stores specific file variable types to communicate with the SD card
	UINT byteswritten;
	FIL file;
	char buffer[20];

	sprintf(buffer, "Memory_%d.txt", M);												// Sets the file name by setting buffer to the memory position

	osMutexWait(myMutex01Handle, osWaitForever);										// SD card reading and writing is protected my mutex because it is a common shared resource

	if((res = f_open(&file, buffer, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)				// Opens the file (&file), with the name (buffer), and in the mode to create and write to, while catching an error 
	{
		safe_printf("ERROR: Opening '%s'\n", buffer);
		return 1;
	}
	safe_printf("Opened file '%s'\n", buffer);

	if ((res = f_write(&file, data, XSIZE*sizeof(uint16_t), &byteswritten)) != FR_OK)	// Writes to the file (&file), writes in (data), of size of (182 x sizeof(uint16_t) each) and captures how many bytes were written
	{
		safe_printf("ERROR: Writing '%s'\n", buffer);
		f_close(&file);																	// Closes file if there was an error in writing
		return 1;
	}
	
	safe_printf("Data of size(%d bytes) was written to Memory_%d\n", byteswritten, M);
	
	f_close(&file);																		// Closes the file

	osMutexRelease(myMutex01Handle);
	return 0;
}

// Function: Read File
// Input: A memory position 
// Output: Loads data from the memory to the LCD screen
uint8_t read_file(uint32_t M){
	FIL file;																			// Stores specific file variable types to communicate with the SD card
	FRESULT res;
	char buffer[100];
	UINT bytesread;
	uint16_t data[XSIZE];

	sprintf(buffer,"Memory_%d.txt", M);													// Sets the file name by setting buffer to the memory position
	
	osMutexWait(myMutex01Handle, osWaitForever);										// SD card reading and writing is protected my mutex because it is a common shared resource
	
	if((res = f_open(&file, buffer, FA_READ)) != FR_OK){								// Opens the file (&file), with the name (buffer), and in the mode to read from, while catching an error 
		safe_printf("Error: opening the file\n");
		return 1;
	}
	safe_printf("Opened file '%s'\n", buffer);
	
	if((res = f_read(&file, data, XSIZE*sizeof(uint16_t), &bytesread)) != FR_OK){		// Reads the file (&file), reads out (data), of size of (182 x sizeof(uint16_t) each) and captures how many bytes were read
		safe_printf("Error: data did not read file\n");
		f_close(&file);																	// Closes file if there was an error in reading
		return 1;
	}

	safe_printf("Data of size(%d bytes) was read from Memory_%d\n", bytesread, M);
	
	f_close(&file);																		// Closes the file

	uint8_t last_xpos = 0;																// Plots data to the LCD screen
	uint8_t last_ypos = 0;
	uint8_t ypos = 0;
	for(uint8_t xpos = 0; xpos < XSIZE; xpos++){
		ypos = data[xpos];																// Uses retrieved data array from SD card
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DrawVLine(XOFF+xpos, YOFF, YSIZE);										// Plots vertical line to erase next position on screen
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_DrawLine(XOFF + last_xpos, YOFF + last_ypos, XOFF+xpos, YOFF+ypos);		// Plots lines making up the graph 
		last_xpos = xpos;
		last_ypos = ypos;
		osMutexRelease(myMutex01Handle);
	}
	return 0;
}
