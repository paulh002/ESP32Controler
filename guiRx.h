#pragma once
#include <lvgl.h>
#include <RotaryEncoder.h>
#include "SwrBarClass.h"

class guiRx
{
public:
	void init(lv_obj_t* tabview, lv_group_t* button_group);
	void barValue(int value);
	String	command{ "VOL" };
	lv_obj_t* button[20]{};
	int button_selected{ -1 };
	void checkButtons(RotaryEncoder& decoder);
	void focus();

private:
	const int swrHeight = 200;
	lv_obj_t* bg_rx, *smeterLabel;
	SwrBarClass SmeterBar;
	lv_style_t style_btn;

	void create_buttons(lv_obj_t* tab, lv_group_t* button_group);
	
};

extern guiRx guirx;