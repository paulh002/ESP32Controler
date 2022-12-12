// 
// 
// 
#include <lvgl.h>
#include "Settings.h"
#include "measurement.h"

Settings settings;


void thresshold_dropdown_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_VALUE_CHANGED)
	{
		R.thresshold = lv_dropdown_get_selected(obj);
		SaveEEPROM();
	}
}


void powerfloor_dropdown_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_VALUE_CHANGED)
	{
		R.low_power_floor = lv_dropdown_get_selected(obj);
		SaveEEPROM();
	}
}

void resetButton_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);


	if (code == LV_EVENT_PRESSED)
	{
		if (settings.getSignal())
		{
			init_measurement(false);
			settings.LoadValues();
			SaveEEPROM();
		}
	}
}

void store1_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);


	if (code == LV_EVENT_PRESSED)
	{
		if (settings.getSignal())
		{
			SaveEEPROM();
		}
	}
}

void store2_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_PRESSED)
	{
		if (settings.getSignal())
		{
			SaveEEPROM();
		}
	}
}

void Settings::LoadValues()
{
	if (R.wifi_onoff)
		lv_obj_add_state(wifiButton, LV_STATE_CHECKED);
	lv_dropdown_set_selected(thresshold_dropdown, R.thresshold);
	lv_spinbox_set_value(ui_dbm40, (int32_t)R.cal_AD[0].db10m);
	lv_spinbox_set_value(ui_dbm10, (int32_t)R.cal_AD[1].db10m);
	lv_dropdown_set_selected(powerfloor_dropdown, R.low_power_floor);
}

void Settings::signalGreen()
{
	lv_obj_set_style_bg_color(SignalPanel, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
	signal = true;
}

void Settings::signalRed()
{
	lv_obj_set_style_bg_color(SignalPanel, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
	signal = false;
}

void Settings::init(lv_obj_t* tabview_tab, std::string name, lv_group_t* button_group)
{
	lv_obj_t* tabview = lv_tabview_add_tab(tabview_tab, name.c_str());

	lv_style_init(&ui_background);
	lv_style_set_radius(&ui_background, 0);
	lv_style_set_bg_color(&ui_background, lv_color_hex(0x464B55)); //lv_color_hex(0x10121D)
	lv_style_set_bg_opa(&ui_background, 255);
	lv_style_set_bg_grad_color(&ui_background, lv_color_hex(0x2D323C)); //lv_color_hex(0x393B46)
	lv_style_set_bg_grad_dir(&ui_background, LV_GRAD_DIR_VER);
	lv_style_set_border_color(&ui_background, lv_color_hex(0x000000));
	lv_style_set_border_opa(&ui_background, 255);
	lv_style_set_border_width(&ui_background, 0);
	lv_style_set_pad_left(&ui_background, 5);
	lv_style_set_pad_right(&ui_background, 5);
	lv_style_set_pad_top(&ui_background, 5);
	lv_style_set_pad_bottom(&ui_background, 5);

	lv_style_set_text_color(&ui_background, lv_color_hex(0xFFFFFF));
	lv_style_set_text_opa(&ui_background, 255);

	lv_obj_add_style(tabview, &ui_background, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_t* ui_Settings_Label7 = lv_label_create(tabview);
	lv_obj_set_width(ui_Settings_Label7, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Settings_Label7, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(ui_Settings_Label7, 270);
	lv_obj_set_y(ui_Settings_Label7, 4);
	lv_label_set_text(ui_Settings_Label7, "Wifi");
	lv_obj_set_style_text_color(ui_Settings_Label7, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Settings_Label7, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	wifiButton = lv_switch_create(tabview);
	lv_obj_set_width(wifiButton, 45);
	lv_obj_set_height(wifiButton, 25);
	lv_obj_set_x(wifiButton, -4);
	lv_obj_set_y(wifiButton, 25);
	lv_obj_set_align(wifiButton, LV_ALIGN_TOP_RIGHT);
	lv_obj_set_style_bg_color(wifiButton, lv_color_hex(0x191D26), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(wifiButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(wifiButton, lv_color_hex(0x191D26), LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(wifiButton, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(wifiButton, LV_BORDER_SIDE_FULL, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(wifiButton, lv_color_hex(0x00D4FF), LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_set_style_bg_opa(wifiButton, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(wifiButton, lv_color_hex(0x7C92BA), LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(wifiButton, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(wifiButton, lv_color_hex(0x536483), LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(wifiButton, LV_GRAD_DIR_VER, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(wifiButton, -5, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(wifiButton, -5, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(wifiButton, -5, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(wifiButton, -5, LV_PART_KNOB | LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(wifiButton, lv_color_hex(0x00FEFF), LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_shadow_opa(wifiButton, 255, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(wifiButton, 5, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_shadow_spread(wifiButton, 0, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_shadow_ofs_x(wifiButton, -3, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_shadow_ofs_y(wifiButton, 0, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_pad_left(wifiButton, -5, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_pad_right(wifiButton, -5, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_pad_top(wifiButton, -5, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_pad_bottom(wifiButton, -5, LV_PART_KNOB | LV_STATE_CHECKED);
	if (R.wifi_onoff)
		lv_obj_add_state(wifiButton, LV_STATE_CHECKED);


	thresshold_dropdown = lv_dropdown_create(tabview);
	lv_dropdown_set_options(thresshold_dropdown, "Off\n1uW\n10uW\n100uW\n1mW\n10mW");
	lv_obj_set_width(thresshold_dropdown, 80);
	lv_obj_set_height(thresshold_dropdown, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(thresshold_dropdown, 0);
	lv_obj_set_y(thresshold_dropdown, 25);
	lv_obj_add_flag(thresshold_dropdown, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_set_style_text_color(thresshold_dropdown, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(thresshold_dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(thresshold_dropdown, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(thresshold_dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_dropdown_set_selected(thresshold_dropdown, R.thresshold);

	lv_obj_set_width(thresshold_dropdown, 80);
	lv_obj_add_event_cb(thresshold_dropdown, thresshold_dropdown_cb, LV_EVENT_VALUE_CHANGED, NULL);

	thressholdLabel = lv_label_create(tabview);
	lv_obj_set_width(thressholdLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(thressholdLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_x(thressholdLabel, 0);
	lv_obj_set_y(thressholdLabel, 5);
	lv_label_set_text(thressholdLabel, "Thresshold");
	lv_obj_set_style_text_color(thressholdLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(thressholdLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	powerfloor_dropdown = lv_dropdown_create(tabview);
	lv_dropdown_set_options(powerfloor_dropdown, "Off\n1uW\n10uW\n100uW\n1mW\n10mW");
	lv_obj_set_width(powerfloor_dropdown, 80);
	lv_obj_set_height(powerfloor_dropdown, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(powerfloor_dropdown, 25);
	lv_obj_set_x(powerfloor_dropdown, lv_pct(30));
	lv_obj_add_flag(powerfloor_dropdown, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_set_style_text_color(powerfloor_dropdown, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(powerfloor_dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(powerfloor_dropdown, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(powerfloor_dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(powerfloor_dropdown, powerfloor_dropdown_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_dropdown_set_selected(powerfloor_dropdown, R.low_power_floor);

	powerfloorLabel = lv_label_create(tabview);
	lv_obj_set_width(powerfloorLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(powerfloorLabel, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_y(powerfloorLabel, 5);
	lv_obj_set_x(powerfloorLabel, lv_pct(30));
	lv_label_set_text(powerfloorLabel, "Powerfloor");
	lv_obj_set_style_text_color(powerfloorLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(powerfloorLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	const int adc_width = 7 * (LV_HOR_RES / 8) + 10;
	const int adc_height = 48;
	
	lv_obj_t *ui_Settings_Label4 = lv_label_create(tabview);
	lv_obj_set_width(ui_Settings_Label4, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(ui_Settings_Label4, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(ui_Settings_Label4, LV_ALIGN_LEFT_MID);
	lv_label_set_text(ui_Settings_Label4, "Signal");
	lv_obj_set_style_text_color(ui_Settings_Label4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_Settings_Label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	SignalPanel = lv_obj_create(tabview);
	lv_obj_set_width(SignalPanel, 17);
	lv_obj_set_height(SignalPanel, 18);
	lv_obj_set_x(SignalPanel, -133);
	lv_obj_set_y(SignalPanel, 20);
	lv_obj_set_align(SignalPanel, LV_ALIGN_CENTER);
	lv_obj_clear_flag(SignalPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_radius(SignalPanel, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(SignalPanel, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(SignalPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(SignalPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(SignalPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_dbm40 = lv_spinbox_create(tabview);
	lv_spinbox_set_range(ui_dbm40, 0, 999);
	lv_spinbox_set_digit_format(ui_dbm40, 3, 2);
	lv_spinbox_step_prev(ui_dbm40);
	lv_spinbox_set_value(ui_dbm40, (int32_t)R.cal_AD[0].db10m);
	lv_group_add_obj(button_group, ui_dbm40);

	lv_obj_set_width(ui_dbm40, 66);
	lv_obj_set_height(ui_dbm40, 36);
	lv_obj_set_x(ui_dbm40, 62);
	lv_obj_set_y(ui_dbm40, 18);
	lv_obj_set_align(ui_dbm40, LV_ALIGN_LEFT_MID);
	lv_obj_clear_flag(ui_dbm40, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(ui_dbm40, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_dbm40, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_dbm40, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_dbm40, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui_dbm40, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_dbm40, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_Store1 = lv_btn_create(tabview);
	lv_obj_set_width(ui_Store1, 66);
	lv_obj_set_height(ui_Store1, 36);
	lv_obj_set_x(ui_Store1, 15);
	lv_obj_set_y(ui_Store1, 17);
	lv_obj_set_align(ui_Store1, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_Store1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_clear_flag(ui_Store1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(ui_Store1, lv_color_hex(0x39395A), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_Store1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(ui_Store1, store1_cb, LV_EVENT_PRESSED, NULL);

	lv_obj_t* label1 = lv_label_create(ui_Store1);
	lv_label_set_text(label1, "store");
	lv_obj_center(label1);
	lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	resetButton = lv_btn_create(tabview);
	lv_obj_set_width(resetButton, 66);
	lv_obj_set_height(resetButton, 36);
	lv_obj_set_x(resetButton, 15 + 66 + 10);
	lv_obj_set_y(resetButton, 17);
	lv_obj_set_align(resetButton, LV_ALIGN_CENTER);
	lv_obj_add_flag(resetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_clear_flag(resetButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(resetButton, lv_color_hex(0x39395A), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(resetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(resetButton, resetButton_cb, LV_EVENT_PRESSED, NULL);

	lv_obj_t* label3 = lv_label_create(resetButton);
	lv_label_set_text(label3, "reset");
	lv_obj_center(label3);
	lv_obj_set_style_text_color(label3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui_dbm10 = lv_spinbox_create(tabview);
	lv_spinbox_set_range(ui_dbm10, 0, 999);
	lv_spinbox_set_digit_format(ui_dbm10, 3, 2);
	lv_spinbox_step_prev(ui_dbm10);
	lv_spinbox_set_value(ui_dbm10, (int32_t)R.cal_AD[1].db10m);
	lv_group_add_obj(button_group, ui_dbm10);
	lv_obj_set_width(ui_dbm10, 66);
	lv_obj_set_height(ui_dbm10, 36);
	lv_obj_set_x(ui_dbm10, 62);
	lv_obj_set_y(ui_dbm10, 61);
	lv_obj_set_align(ui_dbm10, LV_ALIGN_LEFT_MID);
	lv_obj_clear_flag(ui_dbm10, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(ui_dbm10, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_dbm10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui_dbm10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_dbm10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui_dbm10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui_dbm10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);


	ui_Store2 = lv_btn_create(tabview);
	lv_obj_set_width(ui_Store2, 66);
	lv_obj_set_height(ui_Store2, 36);
	lv_obj_set_x(ui_Store2, 16);
	lv_obj_set_y(ui_Store2, 59);
	lv_obj_set_align(ui_Store2, LV_ALIGN_CENTER);
	lv_obj_add_flag(ui_Store2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
	lv_obj_clear_flag(ui_Store2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(ui_Store2, lv_color_hex(0x39395A), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_Store2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(ui_Store2, store2_cb, LV_EVENT_PRESSED, NULL);
	
	lv_obj_t* label2 = lv_label_create(ui_Store2);
	lv_label_set_text(label2, "store");
	lv_obj_center(label2);
	lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_group_add_obj(button_group, thresshold_dropdown);
	lv_group_add_obj(button_group, powerfloor_dropdown);
	lv_group_add_obj(button_group, wifiButton);
	lv_group_add_obj(button_group, ui_dbm40);
	lv_group_add_obj(button_group, ui_Store1);
	lv_group_add_obj(button_group, resetButton);
	lv_group_add_obj(button_group, ui_dbm40);
	lv_group_add_obj(button_group, ui_Store2);
}

void Settings::getFocus()
{
	lv_group_focus_obj(thresshold_dropdown);
}
