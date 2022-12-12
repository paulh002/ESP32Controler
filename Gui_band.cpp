#include "Gui_band.h"
#include "Cat.h"
#include "gui.h"

Gui_band	gui_band;

int			button_width;
int			button_height;
int			button_width_margin;
int			button_height_margin;
int			button_selected = -1;

const lv_coord_t x_margin = 5;
const lv_coord_t y_margin = 5;
const int x_number_buttons = 3;
const int y_number_buttons = 3;
const lv_coord_t tab_margin = 20;

void	Gui_band::init_gui(lv_obj_t* tabview_tab, lv_group_t* button_group)
{
	m_tab = lv_tabview_add_tab(tabview_tab, "Band");

	button_width_margin = ((screenWidth - tab_margin) / x_number_buttons);
	button_width = ((screenWidth - tab_margin) / x_number_buttons) - x_margin;
	button_height = 50;
	button_height_margin = button_height + y_margin;

	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, 10);
	lv_style_set_bg_color(&style_btn, lv_color_make(0x60, 0x60, 0x60));
	lv_style_set_bg_grad_color(&style_btn, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_btn, 255);
	lv_style_set_border_color(&style_btn, lv_color_make(0x9b, 0x36, 0x36));         // lv_color_make(0x2e, 0x44, 0xb2)
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_opa(&style_btn, 255);
	lv_style_set_outline_color(&style_btn, lv_color_black());
	lv_style_set_outline_opa(&style_btn, 255);
	m_button_group = button_group;
}

void button_event_handler1(lv_event_t* e)
{
	gui_band(e);
}

void Gui_band::add_button(std::string s)
{
	if (ibuttons >= nobuttons)
		return;
	
	
	button[ibuttons] = lv_btn_create(m_tab);
	lv_group_add_obj(m_button_group, button[ibuttons]);
	lv_obj_add_style(button[ibuttons], &style_btn, 0);
	lv_obj_add_event_cb(button[ibuttons], button_event_handler1, LV_EVENT_CLICKED, NULL);
	lv_obj_align(button[ibuttons], LV_ALIGN_TOP_LEFT, ibutton_x * button_width_margin, ibutton_y * button_height_margin);
	lv_obj_add_flag(button[ibuttons], LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(button[ibuttons], button_width, button_height);
	lv_label[ibuttons] = lv_label_create(button[ibuttons]);

	s = s + " m";
	lv_label_set_text(lv_label[ibuttons], s.c_str());
	lv_obj_center(lv_label[ibuttons]);
	ibuttons++;

	ibutton_x++;
	if (ibutton_x >= x_number_buttons)
	{
		ibutton_x = 0;
		ibutton_y++;
	}
}

void	Gui_band::reset_button()
{
	ibutton_x = 0;
	ibutton_y = 0;
	if (ibuttons == 0)
		return;
	for (int i = 0; i < ibuttons; i++)
	{
		lv_obj_del(lv_label[i]);
		lv_obj_del(button[i]);
		button[ibuttons] = nullptr;
		lv_label[ibuttons] = nullptr;
	}
	ibuttons = 0;
}

void Gui_band::set_button(int band)
{
	char str[30], str1[30];

	sprintf(str, "%d m", band);
	sprintf(str1, "%d cm", band);
	for (int i = 0; i < ibuttons; i++)
	{
		lv_obj_t* label = lv_obj_get_child(button[i], 0L);
		char* ptr = lv_label_get_text(label);
	
		if (ptr != NULL)
		{
			if ((strcmp(ptr, str) == 0) || (strcmp(ptr, str1) == 0))
				lv_obj_add_state(button[i], LV_STATE_CHECKED);
			else
				lv_obj_clear_state(button[i], LV_STATE_CHECKED);
		}
	}

}

void Gui_band::operator()(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);
	lv_obj_t* label = lv_obj_get_child(obj, 0L);
	char* ptr = lv_label_get_text(label);
	
	for (int i = 0; i < ibuttons; i++)
	{
		if (button[i] != obj)
		{
			lv_obj_clear_state(button[i], LV_STATE_CHECKED);
		}
	}
	lv_obj_add_state(obj, LV_STATE_CHECKED);
	if (ptr)
	{
		char str[20];
		
		strcpy(str, ptr);
		char* ptr1 = strchr(str, ' ');
		if (ptr1)
		{
			*ptr1 = '\0';
			CatInterface.SetBnd(atoi(str));
		}
	}

}

bool Gui_band::have_buttons()
{
	if (ibuttons > 0) 
		return true; 
	else 
		return false;
}