#pragma once
#include <string>
#include <PCF8574.h>
#include "FT891_CAT.h"
#include "Gui_band.h"


class BandFilter
{
public:
	void Setup();
	void setBand(std::string band);
};

