#include "guiRx.h"
#include "measurement.h"
#include "Cat.h"
#include "gui.h"
#include "Gui_band.h"

guiRx guirx;

void button_event_handler(lv_event_t* e);

void guiRx::create_buttons(lv_obj_t* tab, lv_group_t* button_group)
{
	int				ibutton_x = 0, ibutton_y = 0;
	
	int button_width_margin = ((screenWidth - tab_margin) / x_number_buttons);
	int button_width = ((screenWidth - tab_margin) / x_number_buttons) - x_margin;
	int button_height = 50;
	int button_height_margin = button_height + y_margin;
	
	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, 10);
	lv_style_set_bg_color(&style_btn, lv_color_make(0x60, 0x60, 0x60));
	lv_style_set_bg_grad_color(&style_btn, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_btn, 255);
	lv_style_set_border_color(&style_btn, lv_color_make(0x9b, 0x36, 0x36));   // lv_color_make(0x2e, 0x44, 0xb2)
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_opa(&style_btn, 255);
	lv_style_set_outline_color(&style_btn, lv_color_black());

	lv_style_set_outline_opa(&style_btn, 255);
	for (int i = 0; i < 3; i++)
	{
		button[i] = lv_btn_create(tab);
		lv_group_add_obj(button_group, button[i]);
		lv_obj_add_style(button[i], &style_btn, 0);
		lv_obj_add_event_cb(button[i], button_event_handler, LV_EVENT_CLICKED, NULL);
		lv_obj_align(button[i], LV_ALIGN_BOTTOM_LEFT, ibutton_x * button_width_margin, ibutton_y * button_height_margin);
		lv_obj_add_flag(button[i], LV_OBJ_FLAG_CHECKABLE);
		lv_obj_set_size(button[i], button_width, button_height);

		lv_obj_t* lv_label = lv_label_create(button[i]);

		char str[20];
		switch (i)
		{
		case 0:
			strcpy(str, "Volume");
			command = String("VOL");
			button_selected = i;
			lv_obj_add_state(button[i], LV_STATE_CHECKED);
			break;
		case 1:
			strcpy(str, "Gain");
			break;
		case 2:
			strcpy(str, "RF");
			break;
		case 3:
			strcpy(str, "Agc");
			break;
		case 4:
			strcpy(str, "Tune");
			break;
		}

		lv_label_set_text(lv_label, str);
		lv_obj_center(lv_label);

		ibutton_x++;
		if (ibutton_x >= x_number_buttons)
		{
			ibutton_x = 0;
			ibutton_y++;
		}
	}
}

void guiRx::init(lv_obj_t* tabview_tab, lv_group_t* button_group)
{
	lv_obj_t* tabview = lv_tabview_add_tab(tabview_tab, "RX");

	smeterLabel = lv_label_create(tabview);
	lv_obj_set_width(smeterLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(smeterLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(smeterLabel, 0);
	lv_obj_set_y(smeterLabel, 20);
	lv_label_set_text(smeterLabel, "S         1     3     5     7     9     20     40     60");
	lv_obj_set_style_text_color(smeterLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(smeterLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	SmeterBar.init(tabview, 6 * (LV_HOR_RES / 8), 30);
	SmeterBar.align(tabview, LV_ALIGN_TOP_MID, 0, 40);
	SmeterBar.SetRange(120);

	create_buttons(tabview, button_group);
}

void guiRx::barValue(int value)
{
	SmeterBar.value(value);
}

void guiRx::focus()
{
	if (button[0] != nullptr)
		lv_group_focus_obj(button[0]);
}


void button_event_handler(lv_event_t* e)
{

	lv_obj_t* obj = lv_event_get_target(e);
	lv_obj_t* label = lv_obj_get_child(obj, 0L);
	char* ptr = lv_label_get_text(label);

	for (int i = 0; i < 3; i++)
	{
		if (guirx.button[i] != obj)
		{
			lv_obj_clear_state(guirx.button[i], LV_STATE_CHECKED);
		}
		else
		{
			if (i != guirx.button_selected)
			{
				guirx.button_selected = i;
				switch (i)
				{
				case 0:
					guirx.command = String("VOL");
					break;
				case 1:
					guirx.command = String("GAIN");
					break;
				case 2:
					guirx.command = String("RF");
					break;
/*				case 3:
					guirx.command = String("AGC");
					break;
				case 4:
					guirx.command = String("TUNE");
					break;
*/
				}
			}
			else
			{
				guirx.button_selected = -1;
				guirx.command = String("");
				lv_obj_clear_state(guirx.button[i], LV_STATE_CHECKED);
			}

		}
	}
}

void guiRx::checkButtons(RotaryEncoder &decoder)
{
	int count = 0;
	if (guirx.button_selected != -1)
	{
		count = (int)decoder.getDirection(1);
		if (count)
			decoder.ClearEncoder(1);
	}

	if (count)
	{
		switch (guirx.button_selected)
		{
		case 0: // Volume
			CatInterface.Setag(count);
			break;
		case 1: // IF Gain
			CatInterface.Setig(count);
			break;
		case 2:// RF Gain
			CatInterface.Setrg(count);
			break;
		}
	}
}