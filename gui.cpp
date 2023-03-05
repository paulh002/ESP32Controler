

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <RotaryEncoder.h>
#include <TCA9548.h>
#include "gui.h"
#include "mainTab.h"
#include "measurement.h"
#include "Settings.h"
#include "Gui_band.h"
#include "Cat.h"
#include "guiRx.h"

/* lvgl globals*/
#define LVGL_TICK_PERIOD 10

/*-------------------------------------------------------
Rotary encoder settings
--------------------------------------------------------*/
#ifdef TCA_AVAILABLE 
TCA9548		tca(0x70);
#endif

ESP32Encoder Encoder;
const int no_rotary_encoders = 2;
#define PCF8754A_I2C 0x3C
#define PCF8754_I2C 0x24

RotaryEncoder decoder(PCF8754_I2C, no_rotary_encoders, RotaryEncoder::LatchMode::TWO03); //0x24
const int	tca_sda = 21;
const int	tca_sdi = 22;
const int	tca_pcf = 2;
const int PCF8754_INT = 35; //35 or 4 (vfo pcb)

/* TFT instance */
TFT_eSPI tft = TFT_eSPI();
const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;
const int bottomHeight = 40;
const int topHeight = 25;
const int nobuttons = 6;
const int bottombutton_width = (screenWidth / nobuttons) - 2;
const int bottombutton_width1 = (screenWidth / nobuttons);
const int tab_size_y = 40; 
int lastPowerEncoding = 0;
const int setting_tab = 3;

lv_disp_draw_buf_t	draw_buf;
lv_color_t	buf[screenWidth * 10];
lv_indev_t* encoder_indev_t;
lv_group_t* button_group;

lv_style_t	text_style;
lv_style_t	page_style;
lv_style_t	style_btn_tab;
lv_style_t	top_style;

lv_obj_t* bg_top;
lv_obj_t* scr;
lv_obj_t* tabview_tab;
lv_obj_t* label_status;
lv_obj_t* label_wifi;
lv_timer_t	*labelTimer = nullptr;



// Interrupt routine for keypress
volatile bool flag = false;
void IRAM_ATTR moved()
{
	flag = true;
}

static void lv_rt_log(const char* buf)
{
	Serial.println(buf);
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);
	tft.pushColors((uint16_t*)&color_p->full, w * h, true);
	tft.endWrite();

	lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
	uint16_t touchX, touchY;

	bool touched = tft.getTouch(&touchX, &touchY, 600);

	if (tft.getRotation() == 1)
	{
		touchX = screenWidth - touchX;
	}

	if (!touched)
	{
		data->state = LV_INDEV_STATE_REL;
	}
	else
	{
		data->state = LV_INDEV_STATE_PR;

		/*Set the coordinates*/
		data->point.x = touchX;
		data->point.y = touchY;
	}
}

volatile lv_indev_state_t	enc_button_state = LV_INDEV_STATE_REL;

void read_encoder(lv_indev_drv_t* indev, lv_indev_data_t* data)
{
	data->enc_diff = (int)decoder.getDirection(0);
	if (data->enc_diff)
		decoder.ClearEncoder(0);
	data->state = enc_button_state;
	if (data->enc_diff > 0)
		data->enc_diff = 1;
	if (data->enc_diff < 0)
		data->enc_diff = -1;
	return;
}

volatile int lastEncoding{}, lastEncoding1{}, lastCat{};
int		total = 0;
int	    tx = 0;
int		value, currentRxtx = 0;

void guiTask(void* arg) {
	
	while (1) {
		CatInterface.checkCAT();
		if (flag)
		{
			decoder.tick();
			if (decoder.IsButtonPressed(0))
				enc_button_state = LV_INDEV_STATE_PR;
			else
				enc_button_state = LV_INDEV_STATE_REL;

			long mtime, mtime1;
			mtime = mtime1 = millis();
			while (mtime1 - mtime < 100)
			{
				decoder.tick();
				mtime1 = millis();
			}
			flag = false;
		}
		
		int count_vfo = Encoder.getCount();
		Encoder.clearCount();
		if (count_vfo)
		{
			int currMillis = millis();
			if ((currMillis - lastEncoding < 3) && abs(count_vfo) < 2)
			{
				total += count_vfo;
			}
			else
			{
				lastEncoding = currMillis;
				total += count_vfo;
				CatInterface.Setft(total);
				total = 0;
			}
		}
		
		guirx.checkButtons(decoder);
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
		lv_timer_handler(); // let the GUI do its work 
		xSemaphoreGive(GuiBinarySemaphore);

		pswr_sync_from_interrupt();
		calc_SWR_and_power();

		if (lv_tabview_get_tab_act(tabview_tab) == setting_tab)
		{
			if (check_input_cal() == CAL_BAD)
			{
				lv_label_set_text(label_status, "insufficient signal level");
				settings.signalRed();
			}
			else
			{
				lv_label_set_text(label_status, "");
				settings.signalGreen();
			}

		}

		int currMillis = millis();
		if (currMillis - lastPowerEncoding > POLL_TIMER)
		{
			uint16_t swr;
			std::string value, unit;

			swr = print_swr();
			
			mainTabSwr.setSwrValue(swr);
			mainTabSwr.setPowerValue(power_mw_pep);
			mainTabSwr.barValue(power_mw_pep);
			lastPowerEncoding = currMillis;
		}

		if (!gui_band.have_buttons() && (currMillis - lastEncoding1 > 2000))
		{
			CatInterface.Getag();
			CatInterface.Getrg();
			CatInterface.Getig();
			CatInterface.Requestinformation(2);
			lastEncoding1 = currMillis;
		}
		guirx.barValue(CatInterface.GetSM());


		//currentRxtx = CatInterface.GetTX();
		int rxtx = digitalRead(TXRX_SWITCH);
		if (!rxtx)
		{
			if (currentRxtx == 0)
			{
				CatInterface.Settx(1);
				lv_tabview_set_act(tabview_tab, 1, LV_ANIM_OFF);
			}
			currentRxtx = 1;
		}
		else
		{
			if (currentRxtx)
			{
				CatInterface.Settx(0);
				lv_tabview_set_act(tabview_tab, 0, LV_ANIM_OFF);
			}
			currentRxtx = 0;
		}
		vTaskDelay(1);
	}
}

void labelTimer_cb(lv_timer_t *timer)
{
	lv_label_set_text(label_status, "");
	labelTimer = nullptr;
}

void updateBottomStatus(lv_palette_t palette, String sString)
{
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_label_set_text(label_status, sString.c_str());
	lv_obj_set_style_bg_color(bg_top, lv_palette_main(palette), LV_PART_MAIN);
	if (!labelTimer)
	{
		labelTimer = lv_timer_create(labelTimer_cb, 5000, nullptr);
		lv_timer_set_repeat_count(labelTimer, 1);
	}

	xSemaphoreGive(GuiBinarySemaphore);
}

void showWifilabel(bool show)
{
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (show)
		lv_obj_clear_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	else
		lv_obj_add_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	xSemaphoreGive(GuiBinarySemaphore);
}

void init_encoders()
	{
	char str[80];

#ifdef TCA_AVAILABLE
	if (tca.begin(tca_sda, tca_sdi, 400000))
		Serial.println("TCA OK");
	else
		Serial.println("Error TCA9548 not found");
	delay(20);
	tca.selectChannel(2);
	delay(20);
#endif
	if (decoder.begin(tca_sda, tca_sdi, 400000))
		Serial.println("PCF OK");

	if (decoder.isConnected())
		Serial.println("PCF OK");
	else
		Serial.println("Error PCF8754 not found");

	sprintf(str, "I2C clock speed %d", Wire.getClock());
	Serial.println(str);
	pinMode(PCF8754_INT, INPUT_PULLUP);//35
	attachInterrupt(PCF8754_INT, moved, FALLING); //35
	decoder.setButtonPorts(5, 4);

	ESP32Encoder::useInternalWeakPullResistors = NONE;
	Encoder.attachHalfQuad(PULSE_INPUT_PIN, PULSE_CTRL_PIN);
	}

void init_gui() {
	TaskHandle_t xHandle = NULL;

	init_encoders();
	lv_log_register_print_cb(lv_rt_log);
	lv_init();

	tft.begin(); /* TFT init */
	tft.setRotation(1);
	tft.fillScreen(TFT_BLACK);

	/*Set the touchscreen calibration data,
	  the actual data for your display can be aquired using
	  the Generic -> Touch_calibrate example from the TFT_eSPI library*/
	uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
	tft.setTouch(calData);

	lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
	/*Initialize the display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	/*Change the following line to your display resolution*/
	disp_drv.hor_res = screenWidth;
	disp_drv.ver_res = screenHeight;
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.draw_buf = &draw_buf;
	lv_disp_t* display = lv_disp_drv_register(&disp_drv);

	/*Initialize the (dummy) input device driver*/
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touchpad_read;
	lv_indev_drv_register(&indev_drv);

	static lv_indev_drv_t indev_drv_enc;
	lv_indev_drv_init(&indev_drv_enc);
	indev_drv_enc.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_enc.read_cb = read_encoder;
	encoder_indev_t = lv_indev_drv_register(&indev_drv_enc);

	lv_theme_t* th = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN), LV_THEME_DEFAULT_DARK, &lv_font_montserrat_14);
	lv_disp_set_theme(display, th);
	scr = lv_scr_act();
	lv_style_init(&page_style);
	lv_style_set_radius(&page_style, 0);
	lv_style_set_bg_color(&page_style, lv_color_black());
	lv_obj_add_style(scr, &page_style, 0);


	lv_style_init(&top_style);
	lv_style_set_radius(&top_style, 0);
	lv_style_set_bg_color(&top_style, lv_palette_main(LV_PALETTE_INDIGO));
	lv_style_set_border_width(&top_style, 0);

	bg_top = lv_obj_create(scr);
	lv_obj_add_style(bg_top, &top_style, 0);
	lv_obj_set_size(bg_top, LV_HOR_RES, topHeight);
	lv_obj_clear_flag(bg_top, LV_OBJ_FLAG_SCROLLABLE);

	label_status = lv_label_create(bg_top);
	lv_label_set_long_mode(label_status, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(label_status, LV_HOR_RES - 20);
	lv_label_set_text(label_status, "top");
	lv_obj_align(label_status, LV_ALIGN_CENTER, 25, 0);
	lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_status, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	label_wifi = lv_label_create(bg_top);
	//lv_label_set_long_mode(label_wifi, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_width(label_wifi, LV_HOR_RES - 30);
	lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);
	lv_obj_align_to(label_wifi, bg_top, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_text_color(label_wifi, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_wifi, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_style_init(&style_btn_tab);
	lv_style_set_radius(&style_btn_tab, 0);
	lv_style_set_bg_color(&style_btn_tab, lv_color_make(0x60, 0x60, 0x60));
	lv_style_set_bg_grad_color(&style_btn_tab, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_btn_tab, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_btn_tab, 255);

	tabview_tab = lv_tabview_create(lv_scr_act(), LV_DIR_BOTTOM, tab_size_y);
	lv_obj_set_pos(tabview_tab, 0, topHeight);
	lv_obj_set_size(tabview_tab, screenWidth, screenHeight - topHeight);
	lv_obj_add_style(tabview_tab, &page_style, 0);
	lv_obj_set_style_pad_hor(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_clear_flag(tabview_tab, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_t* tab_buttons = lv_tabview_get_tab_btns(tabview_tab);
	lv_obj_add_style(tab_buttons, &style_btn_tab, 0);
	lv_obj_add_event_cb(tabview_tab, tabview_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

	button_group = lv_group_create();
	lv_indev_set_group(encoder_indev_t, button_group);

	guirx.init(tabview_tab, button_group);
	mainTabSwr.init(tabview_tab, button_group);
	gui_band.init_gui(tabview_tab, button_group);
	
	lv_obj_t* tab2 = lv_tabview_add_tab(tabview_tab, "Graph");
	settings.init(tabview_tab, "Settings", button_group);
	lv_group_add_obj(button_group, tab_buttons);
	lv_tabview_set_act(tabview_tab, 0, LV_ANIM_OFF);
	CatInterface.begin();
	//guirx.focus();
	lv_group_focus_obj(tabview_tab);
	xTaskCreate(guiTask,"gui",4096 * 4,NULL,2, &xHandle);
}


void tabview_event_handler(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	
	if (code == LV_EVENT_VALUE_CHANGED) 
	{	
		switch (lv_tabview_get_tab_act(lv_obj_get_parent(obj)))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			//settings.getFocus();
			break;

		}
	}
}