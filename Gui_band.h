#pragma once
#include <lvgl.h>
#include <string>
#include "Cat.h"

class Gui_band
{
public:
	void	init_gui(lv_obj_t* o_tab, lv_group_t* button_group);
	void	add_button(std::string s);
	void	reset_button();
	void	operator()(lv_event_t* e);
	bool	have_buttons();
	void	set_button(int band);

private:
	lv_style_t		style_btn;
	lv_obj_t*		button[30];
	lv_obj_t*		lv_label[30];
	lv_obj_t*		m_tab;
	lv_group_t*		m_button_group;
	int				ibuttons;
	int				nobuttons {15};
	int				ibutton_x {0};
	int				ibutton_y {0};
};

extern const int bottomHeight;
extern const int topHeight;
extern const int nobuttons;
extern const int bottombutton_width ;
extern const int bottombutton_width1;
extern const int tab_size_y ;
extern int		 button_width;
extern int		 button_height;
extern int		 button_width_margin;
extern int		 button_height_margin;
extern const lv_coord_t x_margin;
extern const lv_coord_t y_margin;
extern const int x_number_buttons;
extern const int y_number_buttons;
extern const lv_coord_t tab_margin;

extern Gui_band	gui_band;