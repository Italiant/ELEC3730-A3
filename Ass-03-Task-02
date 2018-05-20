//     $Date: 2018-05-10 10:07:15 +1000 (Thu, 10 May 2018) $
// $Revision: 1301 $
//   $Author: Peter $

#include "Ass-03.h"

//
// REPLACE THE EXAMPLE CODE WITH YOUR CODE
//

void Ass_03_Task_02(void const * argument)
{
  uint32_t loop=0;
  uint16_t i;
  uint8_t s[100];
  osEvent event;
  int32_t b;

  safe_printf("Hello from Task 2\n");

  while (1)
  {
      event = osMessageGet(myQueue01Handle, osWaitForever);
      if (event.status == osEventMessage)
      {
	  b = event.value.v;
	  sprintf(s,"Task 2: %d (got %d)",loop,b);
      }
      osMutexWait(myMutex01Handle, osWaitForever);
      BSP_LCD_DisplayStringAt(5,205, s, LEFT_MODE);
      osMutexRelease(myMutex01Handle);
      safe_printf("%s\n", s);
      loop++;
  }
}
