//     $Date: 2018-05-10 10:07:15 +1000 (Thu, 10 May 2018) $
// $Revision: 1301 $
//   $Author: Peter $

#include "Ass-03.h"

//
// REPLACE THE EXAMPLE CODE WITH YOUR CODE
//

void Ass_03_Task_01(void const * argument)
{

  uint32_t loop=0;
  uint8_t ts[100];
  uint16_t i;

  safe_printf("Hello from Task 1\n");

  // Display welcome message
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DisplayStringAt(5,5, (uint8_t*)"ELEC3730 Assignment 3 (v01 $Rev: 1301 $)",LEFT_MODE);
  BSP_LCD_DisplayStringAt(5,20, (uint8_t*)"This is some demonstration code",LEFT_MODE);
  BSP_LCD_DisplayStringAt(5,35, (uint8_t*)"that can be used as a starting point",LEFT_MODE);
  BSP_LCD_DisplayStringAt(5,50, (uint8_t*)"for the assignment.",LEFT_MODE);
  osMutexRelease(myMutex01Handle);

  while (1)
  {
    if (BSP_TP_GetDisplayPoint(&display) == 0)
    {
    if(((display.y < 180) && (display.y >= 60)))
    {
      osMutexWait(myMutex01Handle, osWaitForever);
      BSP_LCD_FillCircle(display.x, display.y, 2);
      osMutexRelease(myMutex01Handle);
      safe_printf("Task 1: %d (touch %3d,%3d)\n", loop,display.x, display.y);

      // STEPIEN: Send message to Task 2
  	  osMessagePut (myQueue01Handle, (uint32_t)display.x, 0);

    sprintf(ts,"Task 1: %d",(int)loop);
    osMutexWait(myMutex01Handle, osWaitForever);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(5,190, ts, LEFT_MODE);
    osMutexRelease(myMutex01Handle);
    loop++;

    }
    }
    // STEPIEN: Signal received from timer
    osSignalWait(1,osWaitForever);
  if (i > 100)
  {
    HAL_GPIO_TogglePin(GPIOD, LD4_Pin); // Toggle LED4
    i=0;
  }
  else
  {
	i++;
  }
  }
}
