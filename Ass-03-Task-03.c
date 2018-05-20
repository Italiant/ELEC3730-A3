//     $Date: 2018-05-10 10:07:15 +1000 (Thu, 10 May 2018) $
// $Revision: 1301 $
//   $Author: Peter $

#include "Ass-03.h"

//
// REPLACE THE EXAMPLE CODE WITH YOUR CODE
//

uint8_t myReadFile();

FATFS SDFatFs;
FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;

void Ass_03_Task_03(void const * argument)
{
  FRESULT res;
  FRESULT status;
  uint8_t c; // [2];
  uint8_t ReadFlag=1;
  uint32_t loop=0;
  uint8_t s[100];

  safe_printf("Hello from Task 3 (press any key)\n");

  // Check if SD card driver available
  if(retSD != 0)
  {
    safe_printf("ERROR: SD card driver not available.");
    ReadFlag = 0;
  }
  else
  {
    safe_printf("SD card driver available.\n");

    // Mount file system
    if((res = f_mount(&SDFatFs, (TCHAR const *)SDPath, 0)) != FR_OK)
    {
      safe_printf("ERROR: Could not mount file system.\n");
      ReadFlag = 0;
    }
    else
    {
      safe_printf("Mounted file system: %s\n", SDPath);
    }
  }

  while (1)
  {
	  c = getchar();
      sprintf(s,"Task 3: %d (got '%c')",loop,c);
      osMutexWait(myMutex01Handle, osWaitForever);
      BSP_LCD_DisplayStringAt(5,220, s, LEFT_MODE);
      osMutexRelease(myMutex01Handle);
      HAL_GPIO_TogglePin(GPIOD, LD3_Pin); // Toggle LED3
      loop++;

      safe_printf("%s",s);
      sprintf(s,"\nRead file from SD card...\n"); // Test over writing string
      safe_printf("\n");

      myReadFile();
  }
}

uint8_t myReadFile()
{
  FRESULT res;
  uint32_t bytesread;
#define BUFF_SIZE 256
  uint8_t rtext[BUFF_SIZE];

  // Open file Hello.txt
  if((res = f_open(&MyFile, "Hello.txt", FA_READ)) != FR_OK)
  {
    safe_printf("ERROR: Opening 'Hello.txt'\n");
    return 1;
  }
  safe_printf("Task 3: Opened file 'Hello.txt'\n");

  // Read data from file
  if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
  {
    safe_printf("ERROR: Reading 'Hello.txt'\n");
    f_close(&MyFile);
    return 1;
  }
  rtext[bytesread] = '\0';
  safe_printf("Task 3: Read: %s\n", rtext);

  // Close file
  f_close(&MyFile);

  // Unmount file system
  // FATFS_UnLinkDriver(SD_Path);

  return 0;
}
