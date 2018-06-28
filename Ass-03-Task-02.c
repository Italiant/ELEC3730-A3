// This task can be used as the main pulse rate application as it takes
// input from the front panel.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Draw the boxes that make up the buttons and take action when the
// buttons are pressed. See suggested updates for the touch panel task
// that checks for button presses. Can do more in that task.

//--------------------- Includes ---------------------
#include "Ass-03.h"

//--------------------- Function Headers ---------------------
void draw_screen();

//--------------------- Defines ---------------------
// Defines coordinates for graph region
#define XOFF 134
#define YOFF 5
#define XSIZE 182
#define YSIZE 188

//--------------------- Global Variables ---------------------
int debug_global = 0;
int M = 0;

//--------------------- Task 2: Main ---------------------
void Ass_03_Task_02(void const * argument)
{
	// Declaring variables
	int loop=0;
	Coordinate display;

	osEvent event;

	// Wait for signal from task 1 to start
	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 2 - Pulse Rate Application (touch screen input)\n");

	draw_screen(); // Draw the display interface

	// Must stay in this while loop to work
	while (1)
	{
		// If touched in this area... Do this... for all of the following statements
		if (getfp(&display) == 0)
		{
			// --------------------- Graph Area --------------------- 
			if((display.y > YOFF+5) && (display.y < YOFF+YSIZE-5) && // Graph box
					(display.x > XOFF+5) && (display.x < XOFF+XSIZE-5))
			{
				osMutexWait(myMutex01Handle, osWaitForever);
				BSP_LCD_FillCircle(display.x, display.y, 2);
				osMutexRelease(myMutex01Handle);
				loop++;
				safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);
			}
			else if((display.x > 3) && (display.x < 128)){

				// --------------------- Start --------------------- 
				if((display.y > 3) && (display.y < 47))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
					BSP_LCD_FillRect(4, 4, 124, 43); // make origin one more and size both one less
					HAL_Delay(200);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(4, 4, 124, 43);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(6, 13, (uint8_t*)"Start",LEFT_MODE); 	// Start: (3,3)>(128,47)
					
					osMutexRelease(myMutex01Handle);

					osMessagePut (myQueue02Handle, 1, 0); // Sends message to task 4 start the plotting the graph			

					if(debug_global){
						safe_printf("Start button pressed\n");
					}
				}
				// --------------------- Stop --------------------- 
				else if((display.y > 51) && (display.y < 95))						
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					BSP_LCD_FillRect(4, 52, 124, 43); // make origin one more and size both one less
					HAL_Delay(200);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(4, 52, 124, 43);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(6, 61, (uint8_t*)"Stop",LEFT_MODE); 	// Stop: (3,51)>(128,95)

					osMessagePut (myQueue02Handle, 0, 0); // Sends message to task 4 stop the plotting the graph

					osMutexRelease(myMutex01Handle);

					if(debug_global){
						safe_printf("Stop button pressed\n");
					}
				}
				// --------------------- Load --------------------- 
				else if((display.y > 99) && (display.y < 143))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
					BSP_LCD_FillRect(4, 100, 124, 43); // make origin one more and size both one less
					HAL_Delay(200);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(4, 100, 124, 43);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(6, 109, (uint8_t*)"Load",LEFT_MODE); 	// Load: (3,99)>(128,143)

					osMutexRelease(myMutex01Handle);

					osMessagePut(myQueue05Handle, M, 0);

					if(debug_global){
						safe_printf("Load button pressed\n");
					}
				}
				// --------------------- Store --------------------- 
				else if((display.y > 147) && (display.y < 191))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
					BSP_LCD_FillRect(4, 148, 124, 43); // make origin one more and size both one less
					HAL_Delay(200);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(4, 148, 124, 43);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(6, 157, (uint8_t*)"Store",LEFT_MODE); // Store: (3,147)>(128,191)

					osMutexRelease(myMutex01Handle);

					osMessagePut(myQueue04Handle, M, 0); // Sends memory position to task 4 and triggers function to store data

					if(debug_global){
						safe_printf("Store button pressed\n");
					}
				}
				// --------------------- Debug --------------------- 
				else if((display.y > 195) && (display.y < 237))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
					BSP_LCD_FillRect(4, 196, 124, 41); // make origin one more and size both one less
					HAL_Delay(200);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(4, 196, 124, 41);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(6, 205, (uint8_t*)"Debug",LEFT_MODE); // Debug: (3,195)>(128,237)

					osMutexRelease(myMutex01Handle);

					debug_global = !debug_global;
					if(debug_global){
						safe_printf("Debug messages will be displayed\n");
					}else{
						safe_printf("Debug messages will not be displayed\n");
					}
				}
			}else if((display.y >= 208) && (display.y <= 238)){
				// --------------------- M1 ---------------------
				if((display.x > 139) && (display.x < 193))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
					BSP_LCD_FillRect(140, 209, 51, 27); // make origin one more and size both one less
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(142, 211, (uint8_t*)"M1",LEFT_MODE);

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(198, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(199, 211, (uint8_t*)"M2",LEFT_MODE); 	// M2: (196,208)>(249,237)

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(254, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(256, 211, (uint8_t*)"M3",LEFT_MODE); 	// M3: (253,208)>(306,237)

					osMutexRelease(myMutex01Handle);

					M = 1;

					if(debug_global){
						safe_printf("M1 button pressed\n");
						safe_printf("Memory = (%d)\n", M);
					}
				}
				// --------------------- M2 ---------------------
				else if((display.x > 197) && (display.x < 248))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
					BSP_LCD_FillRect(197, 209, 51, 27); // make origin one more and size both one less
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(199, 211, (uint8_t*)"M2",LEFT_MODE); 	// M2: (196,208)>(249,237)

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(140, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(142, 211, (uint8_t*)"M1",LEFT_MODE);

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(254, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(256, 211, (uint8_t*)"M3",LEFT_MODE); 	// M3: (253,208)>(306,237)

					osMutexRelease(myMutex01Handle);

					M = 2;

					if(debug_global){
						safe_printf("M1 button pressed\n");
						safe_printf("Memory = (%d)\n", M);
					}
				}
				// --------------------- M3 ---------------------
				else if((display.x > 254) && (display.x < 305))
				{
					osMutexWait(myMutex01Handle, osWaitForever);

					BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
					BSP_LCD_FillRect(254, 209, 51, 27); // make origin one more and size both one less
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(256, 211, (uint8_t*)"M3",LEFT_MODE); 	// M3: (253,208)>(306,237)

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(198, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(199, 211, (uint8_t*)"M2",LEFT_MODE); 	// M2: (196,208)>(249,237)

					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(140, 209, 51, 27);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					BSP_LCD_DisplayStringAt(142, 211, (uint8_t*)"M1",LEFT_MODE);

					osMutexRelease(myMutex01Handle);

					M = 3;

					if(debug_global){
						safe_printf("M1 button pressed\n");
						safe_printf("Memory = (%d)\n", M);
					}
				}
			}
		}else{
			// For now... do nothing if touch screen is not pressed, pretty sure this is not a fix for above problem
		}
	}
}

// --------------------- Functions --------------------- 
void draw_screen(){
	// Display LED screen layout
	// ------------------------------------------------------------------------------
	osMutexWait(myMutex01Handle, osWaitForever);

	// Draw rectangles
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	int xpos = 3;
	for(int i = 0; i < 4; i++, xpos += 48){
		BSP_LCD_DrawRect(3, xpos, 125, 44);
	}
	BSP_LCD_DrawRect(3, 195, 125, 42);

	BSP_LCD_DrawRect(132, 3, 185, 189);
	BSP_LCD_DrawRect(133, 4, 185, 189);
	BSP_LCD_DrawRect(139, 208, 53, 29);
	BSP_LCD_DrawRect(196, 208, 53, 29);
	BSP_LCD_DrawRect(253, 208, 53, 29);

	// Draw straight lines
	// Horizontal
	int xypos = 48;
	for(int i = 0; i < 4; i++, xypos += 48){
		BSP_LCD_DrawHLine(3, xypos, 125);
	}
	BSP_LCD_DrawHLine(3, 238, 125);

	BSP_LCD_DrawHLine(139, 238, 54);
	BSP_LCD_DrawHLine(196, 238, 54);
	BSP_LCD_DrawHLine(253, 238, 54);

	// Vertical
	int y1pos = 3;
	for(int i = 0; i < 4; i++, y1pos += 48){
		BSP_LCD_DrawVLine(129, y1pos, 45);
	}
	BSP_LCD_DrawVLine(129, 195, 43);

	BSP_LCD_DrawVLine(193, 208, 30);
	BSP_LCD_DrawVLine(250, 208, 30);
	BSP_LCD_DrawVLine(307, 208, 30);

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
	BSP_LCD_DisplayStringAt(283, 195, (uint8_t*)"10s", LEFT_MODE); // 10s: (133,4)>(318,192)

	osMutexRelease(myMutex01Handle);
	// ------------------------------------------------------------------------------
}


