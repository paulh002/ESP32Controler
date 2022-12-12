// Settings.h

#ifndef _Settings_h
#define _Settings_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Settings
{
 protected:


 public:
	void init(lv_obj_t* tabview_tab, std::string name, lv_group_t* button_group);
	void getFocus();
	void signalRed();
	void signalGreen();
	bool getSignal() { return signal; }
	void LoadValues();


private:
	lv_style_t text_style, ui_background;
	lv_obj_t* thressholdLabel, *thresshold_dropdown, *powerfloor_dropdown, * powerfloorLabel, *bg_adc1;
	lv_obj_t* ui_dbm40, *SignalPanel, *ui_Store1, * ui_dbm10, *ui_Store2;
	lv_obj_t* wifiButton, *resetButton;
	bool signal {false};
};

extern Settings settings;

extern void SaveEEPROM();

#endif

