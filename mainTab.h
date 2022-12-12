#pragma once
#include <lvgl.h>
#include "SwrBarClass.h"

class mainTab
{
public:
	void init(lv_obj_t* tabview, lv_group_t* button_group);
	void setPowerValue(double pwrValue);
	void setSwrValue(uint16_t swrValue);
	void barValue(double value);
private:
	const int swrHeight = 200;
	lv_obj_t *bg_swr, *bg_pwr, *swr, *pwr,* pwr_unit, *swr_unit, *pwr_label;
	SwrBarClass SwrBar;
};

extern mainTab	mainTabSwr;