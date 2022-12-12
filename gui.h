// gui.h

#ifndef _gui_h
#define _gui_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
extern const uint32_t screenWidth;
extern const uint32_t screenHeight;
extern const int bottomHeight;
extern const int topHeight;
extern const int nobuttons;
extern const int bottombutton_width;
extern const int bottombutton_width1;
extern const int tab_size_y;

extern SemaphoreHandle_t GuiBinarySemaphore;

extern void init_gui(void);
extern void updateBottomStatus(lv_palette_t color, String sString);
extern void showWifilabel(bool show);
extern void tabview_event_handler(lv_event_t* e);
#endif

