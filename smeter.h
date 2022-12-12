// smeter.h

#ifndef _smeter_h
#define _smeter_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSmeter
{
public:
	void init(lv_obj_t* parent);
	//void align(lv_obj_t* parent, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
	void value(double value);

private:
	lv_obj_t* meter;
	lv_meter_indicator_t* smeter_indic;
	lv_style_t meter_style, box_style;
};

extern CSmeter Smeter;
#endif

