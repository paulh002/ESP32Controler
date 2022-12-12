// SwrBarClass.h

#ifndef _SwrBarClass_h
#define _SwrBarClass_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <lvgl.h>

class SwrBarClass
{
 protected:


 public:
	void init(lv_obj_t *parent, lv_coord_t w, lv_coord_t h);
	void align(lv_obj_t* parent, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
	void value(int value);
	void SetRange(int range);

private:
	int maxvalue = 66; // 4000 Watt
	lv_style_t style_bg;
	lv_style_t style_indic;
	lv_obj_t* bar;
};


#endif

