//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

// This is the task that reads the analog input. A buffer is divided in two to
// allow working on one half of the buffer while the other half is being
// loaded using the DMA controller.
//
// This task also plots the wave form to the screen.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Note that there needs to be a way of starting and stopping the display.

uint16_t ADC_Value[1000];

void Ass_03_Task_04(void const * argument)
{
	uint16_t i;
	HAL_StatusTypeDef status;
	uint16_t xpos=0;
	uint16_t ypos=0;
	uint16_t last_xpos=0;
	uint16_t last_ypos=0;

	// osEvent event;
	uint32_t start = 1;
	uint32_t analog;

#define XOFF 134
#define YOFF 5
#define XSIZE 182
#define YSIZE 188

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

	// Draw a box to plot in
	/* osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
  osMutexRelease(myMutex01Handle);*/

	// Start the conversion process
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 1000);
	if (status != HAL_OK)
	{
		safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
	}

	osEvent event1, event2;

	// Start main loop
	while (1)
	{
		event1 = osMessageGet(myQueue02Handle, 5);
		if (event1.status == osEventMessage)
		{
			start = event1.value.v;
		}

		event2 = osMessageGet(myQueue03Handle, 5);
		if (event2.status == osEventMessage)
		{
			analog = event2.value.v;
			safe_printf("Plotting time changed to (%d)s\n", analog);
		}

		if(start == 1){

			// Wait for first half of buffer
			osSemaphoreWait(myBinarySem05Handle, osWaitForever);
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
				xpos++;
			}
			osMutexRelease(myMutex01Handle);
			if (last_xpos>=XSIZE-1)
			{
				xpos=0;
				last_xpos=0;
			}

			// Wait for second half of buffer
			osSemaphoreWait(myBinarySem06Handle, osWaitForever);
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
				xpos++;
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

		}
	}
}

// STEPIEN: Add callback functions to see if this can be used for double buffering equivalent

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem05Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
	osSemaphoreRelease(myBinarySem06Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
}

