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

uint16_t ADC_Value[1000];

//--------------------- Function Headers ---------------------
//uint8_t myWriteFile(uint16_t *data, int M1);

//--------------------- Defines ---------------------
// Defines coordinates for graph region
#define XOFF 134
#define YOFF 5
#define XSIZE 182
#define YSIZE 187

//--------------------- Global Variables ---------------------

//--------------------- Task 4: Main ---------------------
void Ass_03_Task_04(void const * argument)
{
	// Declare variables
	uint16_t i;
	uint16_t data[200];
	HAL_StatusTypeDef status;
	uint16_t xpos=0;
	uint16_t ypos=0;
	uint16_t last_xpos=0;
	uint16_t last_ypos=0;
	uint32_t start = 1;
	uint32_t analog = 10;
	uint8_t memory = 0;

	osEvent event1, event2, event3;

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
		event1 = osMessageGet(myQueue02Handle, 5);
		if (event1.status == osEventMessage)
		{
			start = event1.value.v;
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

		// Wait for message from task 2 to receive memory position
		event3 = osMessageGet(myQueue04Handle, 5);
		if (event3.status == osEventMessage)
		{
			memory = event3.value.v;

			safe_printf("File memory (%d) selected\n", memory);
			// myWriteFile(data, memory);
		}

		if(start){ // Used to start and stop plotting the graph
			//osTimerStart(myTimer02Handle, 1);

			// Wait for first half of buffer
			osSemaphoreWait(myBinarySem05Handle, 1000/(18.2/(analog/10)));
			//osSemaphoreWait(myBinarySem05Handle, osWaitForever);
			osMutexWait(myMutex01Handle, osWaitForever);
			for(i=0;i<500;i=i+500)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				ypos=(uint16_t)((uint32_t)(ADC_Value[i])*YSIZE/4096);
				BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
				// BSP_LCD_FillRect(xpos,ypos,1,1);
				last_xpos=xpos;
				last_ypos=ypos;
				data[xpos] = ypos;
				xpos += 1;
			}
			osMutexRelease(myMutex01Handle);
			if (last_xpos>=XSIZE-1)
			{
				xpos=0;
				last_xpos=0;
			}

			// Wait for second half of buffer
			osSemaphoreWait(myBinarySem06Handle, 1000/(18.2/(analog/10)));
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
				data[xpos] = ypos;
				xpos += 1;
			}
			osMutexRelease(myMutex01Handle);
			if (last_xpos>=XSIZE-1)
			{
				xpos=0;
				last_xpos=0;
			}
			HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);

		}
		else{
			//osTimerStart(myTimer02Handle, 1);
			//osTimerStart(myTimer03Handle, analog*1000);
			// If start is = 0 then do not plot the graph, pause it
		}
	}
}

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

//uint8_t WriteFile(uint16_t *data, int M1)
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

