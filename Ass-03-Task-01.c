//     $Date: 2018-05-10 10:07:15 +1000 (Thu, 10 May 2018) $
// $Revision: 1301 $
//   $Author: Peter $

#include "Ass-03.h"

void Ass_03_Task_01(void const * argument)
{

  uint32_t loop=0;
  uint8_t ts[100];
  uint16_t i;

  safe_printf("Hello from Task 1\n");

  // Display LED screen layout
  // ------------------------------------------------------------------------------
  osMutexWait(myMutex01Handle, osWaitForever);

  // Initialize and turn on LED screen
  BSP_LCD_Init();
  BSP_LCD_DisplayOn();
  BSP_TP_Init();
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  // Draw rectangles
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  int xpos = 3, ypos = 47;
  for(int i = 0; i < 4; i++, xpos += 48, ypos += 48){
	  BSP_LCD_DrawRect(3, xpos, 128, ypos);
  }
  BSP_LCD_DrawRect(3, 195, 128, 237);

  BSP_LCD_DrawRect(132, 3, 317, 191);
  BSP_LCD_DrawRect(133, 4, 318, 192);
  BSP_LCD_DrawRect(139, 208, 192, 237);
  BSP_LCD_DrawRect(196, 208, 249, 237);
  BSP_LCD_DrawRect(253, 208, 306, 237);

  // Draw straight lines
  // Horizontal
  int xypos = 48;
  for(int i = 0; i < 4; i++, xypos += 48){
	  BSP_LCD_DrawHLine(3, xypos, 129, xypos);
  }
  BSP_LCD_DrawHLine(3, 238, 129, 238);

  BSP_LCD_DrawHLine(139, 238, 193, 238);
  BSP_LCD_DrawHLine(196, 238, 250, 238);
  BSP_LCD_DrawHLine(253, 238, 307, 238);

  // Vertical
  int y1pos = 3, y2pos = 48;
  for(int i = 0; i < 4; i++, y1pos += 48, y2pos += 48){
  	BSP_LCD_DrawVLine(129, y1pos, 129, y2pos);
  }
  BSP_LCD_DrawVLine(129, 195, 129, 238);

  BSP_LCD_DrawVLine(193, 208, 193, 238);
  BSP_LCD_DrawVLine(250, 208, 250, 238);
  BSP_LCD_DrawVLine(307, 208, 307, 238);

  // Write text
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(6, 13, (uint8_t*)"Start",LEFT_MODE); 	// Start: (3,3)>(128,47)
  BSP_LCD_DisplayStringAt(6, 61, (uint8_t*)"Stop",LEFT_MODE); 	// Stop: (3,51)>(128,95)
  BSP_LCD_DisplayStringAt(6, 109, (uint8_t*)"Load",LEFT_MODE); 	// Load: (3,99)>(128,143)
  BSP_LCD_DisplayStringAt(6, 157, (uint8_t*)"Store",LEFT_MODE); // Store: (3,147)>(128,191)
  BSP_LCD_DisplayStringAt(6, 205, (uint8_t*)"Debug",LEFT_MODE); // Debug: (3,195)>(128,237)

  BSP_LCD_DisplayStringAt(142, 211, (uint8_t*)"M1",LEFT_MODE); 	// M1: (139,208)>(192,237)
  BSP_LCD_DisplayStringAt(199, 211, (uint8_t*)"M2",LEFT_MODE); 	// M2: (196,208)>(249,237)
  BSP_LCD_DisplayStringAt(256, 211, (uint8_t*)"M3",LEFT_MODE); 	// M3: (253,208)>(306,237)

  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(132, 195, (uint8_t*)"0s",LEFT_MODE); 	// 0s: (132,3)>(317,191)
  BSP_LCD_DisplayStringAt(238, 195, (uint8_t*)"10s",LEFT_MODE); // 10s: (133,4)>(318,192)

  osMutexRelease(myMutex01Handle);
  // ------------------------------------------------------------------------------

  while (1)
  {
    /*if (BSP_TP_GetDisplayPoint(&display) == 0)
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
    */
  if (i > 100)
  {
    HAL_GPIO_TogglePin(GPIOD, LD4_Pin); // Toggle LED4
    i=0;
  }
  else
  {
	i++;
  }

  }// end of while(1) loop

}// end of task 1
