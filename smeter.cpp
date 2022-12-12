#include <lvgl.h>
#include "smeter.h"

CSmeter Smeter;

static void smeter_event_cb(lv_event_t* e)
{
	lv_event_code_t			 code = lv_event_get_code(e);
	lv_obj_draw_part_dsc_t* dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);

	switch (code)
	{
	case LV_EVENT_DRAW_PART_BEGIN:
		dsc->value = dsc->value / 10;
		if (dsc->value == 1)
		{
			strcpy(dsc->text, "S");
		}

		if (dsc->value > 9)
		{
			if (dsc->value == 10)
				dsc->value = 20;
			if (dsc->value == 11)
				dsc->value = 40;
			if (dsc->value == 12)
				dsc->value = 60;
		}
		lv_snprintf(dsc->text, sizeof(dsc->text), "%d", dsc->value);
		break;
	}
}


void CSmeter::init(lv_obj_t* tabview)
{
	lv_obj_t* box = lv_tabview_add_tab(tabview, "S");

	lv_coord_t x = 0; lv_coord_t y = 3;
	lv_coord_t w = lv_obj_get_width(tabview) - 3;
	lv_coord_t h = lv_obj_get_height(tabview) - 3;
	
	lv_style_init(&box_style);
	lv_style_set_radius(&box_style, 0);
	lv_style_set_bg_color(&box_style, lv_color_black());
	lv_style_set_border_color(&box_style, lv_color_black());
	lv_obj_set_style_pad_hor(box, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(box, 0, LV_PART_MAIN);
	lv_style_set_border_width(&box_style, 0);
	lv_style_set_outline_color(&box_style, lv_color_black());
	lv_style_set_outline_width(&box_style, 0);
	lv_obj_add_style(box, &box_style, 0);
	lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

	lv_style_init(&meter_style);
	lv_style_set_radius(&meter_style, 0);
	lv_style_set_bg_color(&meter_style, lv_color_black());
	lv_style_set_outline_color(&meter_style, lv_color_black());
	lv_style_set_outline_width(&meter_style, 0);
	lv_style_set_border_width(&meter_style, 0);
	lv_style_set_border_color(&meter_style, lv_color_black());
	lv_style_set_text_color(&meter_style, lv_palette_main(LV_PALETTE_LIME));

	meter = lv_meter_create(box);
	lv_obj_set_pos(meter, x, y);
	lv_obj_set_size(meter, w, h);
	lv_obj_add_style(meter, &meter_style, 0);
	lv_obj_add_event_cb(meter, smeter_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

	/*Remove the circle from the middle*/
	//lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);
	lv_obj_set_style_pad_hor(meter, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(meter, 0, LV_PART_MAIN);
	lv_obj_set_style_size(meter, w, LV_PART_MAIN);

	/*Add a scale first*/
	lv_meter_scale_t* scale = lv_meter_add_scale(meter);
	lv_meter_set_scale_range(meter, scale, 10, 120, 100, 220);
	lv_meter_set_scale_ticks(meter, scale, 12, 10, 5, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
	lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 10, lv_color_hex3(0xeee), 10);

	//lv_meter_set_scale_major_ticks(meter, scale, 1, 4, 10, lv_color_hex3(0xeee), 10);

	lv_meter_indicator_t* indic;
	indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_GREEN), 0);
	lv_meter_set_indicator_start_value(meter, indic, 0);
	lv_meter_set_indicator_end_value(meter, indic, 90);

	lv_meter_indicator_t* indic1;
	indic1 = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
	lv_meter_set_indicator_start_value(meter, indic1, 90);
	lv_meter_set_indicator_end_value(meter, indic1, 120);


	smeter_indic = lv_meter_add_needle_line(meter, scale, 1, lv_color_white(), -10);
	lv_meter_set_indicator_value(meter, smeter_indic, 0);
}

void CSmeter::value(double value)
{
	//printf("value %f\n", value);
	//value = 30.0 + value;
	//value = value + 200.0;
	//printf(" value s%f \n", value);
	lv_meter_set_indicator_value(meter, smeter_indic, value);
}