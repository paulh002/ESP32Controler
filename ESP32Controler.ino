/*
 Name:		ESPBleControler.ino
 Created:	7/21/2021 11:43:23 PM
 Author:	paulh
*/
#include <Wire.h>
#include <WiFi.h>
#include "network.h"
#include "credentials.h"
#include "setup.h"
#include <ESP32Encoder.h>
#include <RotaryEncoder.h>


#include <lvgl.h>
#include <TFT_eSPI.h>
#include <TCA9548.h>
#include "FT891_CAT.h"
#include "Cat.h"
#include "Gui_band.h"

/*-------------------------------------------------------
   Optical Rotary encoder settings (used for frequency)
--------------------------------------------------------*/
#define PULSE_INPUT_PIN 23  // Rotaty Encoder A
#define PULSE_CTRL_PIN  17  // Rotaty Encoder B
#define TXRX_SWITCH   26

/*Cat interface*/
Cat			CatInterface;
Gui_band	gui_band;
TCA9548		tca(0x70);

const int	tca_sda = 21;
const int	tca_sdi = 22;
const int	tca_pcf = 2;

/*-------------------------------------------------------
   Mechanical Rotary encoder settings
--------------------------------------------------------*/
const int	no_rotary_encoders = 2;
RotaryEncoder decoder(0x24, no_rotary_encoders, RotaryEncoder::LatchMode::TWO03);

volatile bool flag = false;
void IRAM_ATTR moved()
{
	flag = true;
}

/*-------------------------------------------------------
   Optical Rotary encoder settings
--------------------------------------------------------*/
ESP32Encoder    Enc_vfo;

// Wifi
var_t	R;

SemaphoreHandle_t GuiBinarySemaphore = NULL;

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 240;
const int bottomHeight = 40;
const int topHeight = 25;
const int nobuttons = 6;
const int bottombutton_width = (screenWidth / nobuttons) - 2;
const int bottombutton_width1 = (screenWidth / nobuttons);
const int tab_size_y = 40;

const lv_coord_t x_margin = 5;
const lv_coord_t y_margin = 5;
const int x_number_buttons = 3;
const int y_number_buttons = 3;
const lv_coord_t tab_margin = 20;

int			button_width;
int			button_height;
int			button_width_margin;
int			button_height_margin;
int			button_selected = -1;

static lv_disp_draw_buf_t	draw_buf;
static lv_color_t			buf[screenWidth * 10];

lv_obj_t*	scr, *bg_page1, *button[20], *button_filter[10];
lv_obj_t*	tabview_tab;
lv_style_t	text_style;
lv_style_t	page_style;
lv_style_t	style_btn;

lv_group_t	*button_group[5];
lv_indev_t	*encoder_indev_t;

String	command;

void button_event_handler(lv_event_t* e);
void create_tab_filter(lv_obj_t* tab, lv_group_t* button_group);
void create_tab_setting(lv_obj_t* tab, lv_group_t* button_group);
static void scroll_begin_event(lv_event_t* e);

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


static	int tab_active = 0; 

void set_next_tab()
{
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	tab_active++;
	if (tab_active > 3)
		tab_active = 0;
	lv_tabview_set_act(tabview_tab, tab_active, LV_ANIM_ON);
	switch (tab_active)
	{
	case 0:
		lv_indev_set_group(encoder_indev_t, button_group[0]);
		break;
	case 3:
		lv_indev_set_group(encoder_indev_t, button_group[1]);
		lv_group_focus_obj(button_filter[0]);
		break;
	}
	xSemaphoreGive(GuiBinarySemaphore);


}

// the setup function runs once when you press reset or power the board
void setup() {
	char	str[80];

	Serial.begin(115200); 
	
	if (tca.begin(tca_sda, tca_sdi, 400000))
		Serial.println("TCA OK");
	else
		Serial.println("Error TCA9548 not found");
	delay(20);
	//tca.selectChannel(2);
	delay(20);
	if (decoder.isConnected())
		Serial.println("PCF OK");
	else
		Serial.println("Error PCF8754 not found");

	sprintf(str, "I2C clock speed %d", Wire.getClock());
	Serial.println(str);
	pinMode(35, INPUT_PULLUP);
	attachInterrupt(35, moved, FALLING);
	decoder.setButtonPorts(5, 4);

	GuiBinarySemaphore = xSemaphoreCreateMutex();
	if (GuiBinarySemaphore == NULL) {
		Serial.println("Error creating the GuiBinarySemaphore");
	}
	pinMode(TXRX_SWITCH, INPUT);
	
	ESP32Encoder::useInternalWeakPullResistors = NONE;
	Enc_vfo.attachHalfQuad(PULSE_INPUT_PIN, PULSE_CTRL_PIN);
// setup display and lvgl
	
	lv_init();

	tft.begin();          /* TFT init */
	tft.setRotation(1); /* Landscape orientation, flipped */

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
	lv_disp_t *display = lv_disp_drv_register(&disp_drv);

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

	scr = lv_scr_act();
	lv_theme_t* th = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN), LV_THEME_DEFAULT_DARK, &lv_font_montserrat_14);
	lv_disp_set_theme(display, th);
	
	lv_style_init(&page_style);
	lv_style_set_radius(&page_style, 0);
	lv_style_set_bg_color(&page_style, lv_color_black());
	lv_obj_add_style(scr, &page_style, 0);

	tabview_tab = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, tab_size_y);
	lv_obj_set_pos(tabview_tab, 0, 0);
	lv_obj_set_size(tabview_tab, screenWidth, screenHeight);
	lv_obj_add_style(tabview_tab, &page_style, 0);
	lv_obj_set_style_pad_hor(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_clear_flag(tabview_tab, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t* tab1 = lv_tabview_add_tab(tabview_tab, "Band");
	lv_obj_t* tab2 = lv_tabview_add_tab(tabview_tab, "Filter");
	lv_obj_t* tab3 = lv_tabview_add_tab(tabview_tab, "Settings");
	//lv_obj_t* tab3 = lv_tabview_add_tab(tabview_tab, "Freq");

	button_group[0] = lv_group_create();
	button_group[1] = lv_group_create();
	button_group[2] = lv_group_create();

	gui_band.init_gui(tab1, button_group[0]);
	
	lv_obj_clear_flag(lv_tabview_get_content(tabview_tab), LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb(lv_tabview_get_content(tabview_tab), scroll_begin_event, LV_EVENT_SCROLL_BEGIN, NULL);


	static lv_style_t background_style;

	lv_style_init(&background_style);
	lv_style_set_radius(&background_style, 0);


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
	lv_style_set_border_color(&style_btn, lv_color_make(0x9b, 0x36, 0x36));   // lv_color_make(0x2e, 0x44, 0xb2)
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_opa(&style_btn, 255);
	lv_style_set_outline_color(&style_btn, lv_color_black());
	lv_style_set_outline_opa(&style_btn, 255);


	//lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tabview_tab);

	//lv_group_add_obj(button_group[0], tabview_tab);
	lv_indev_set_group(encoder_indev_t, button_group[0]);

	create_tab_filter(tab2, button_group[1]); 
	create_tab_setting(tab3, button_group[2]);
	

	lv_obj_clear_flag(lv_tabview_get_content(tabview_tab), LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_ONE);

	CatInterface.begin();
	CatInterface.Getag();
	CatInterface.Getrg();
	CatInterface.Requestinformation(2);

	memset(&R, 0, sizeof(R));
	R.wifi_onoff = true;
	if (strlen(R.ssid[0]) == 0) // If no ssid configured take from credentials.h
	{
		for (int i = 0; i < NUMBR_SSID; i++)
		{
			strcpy(R.ssid[i], _ssid[i]);
			strcpy(R.password[i], _password[i]);
		}
	}
	if (R.wifi_onoff)
	{
		for (int i = 0; i < NUMBR_SSID; i++)
		{
			if (strlen(R.ssid[i]))
			{
				Serial.println("Start Wifi");
				if (vfo_network.begin(TRXNET_SERVER))
				{
					return;
				}
			}
		}
	}
}

volatile int lastEncoding = 0, lastEncoding1 = 0;
int		total = 0;
int	    tx = 0;
int		value;

void loop() 
{
	vfo_network.network_loop();
	CatInterface.checkCAT(); // Check if sdrberry has send any information
	if (flag)
	{
		long mtime, mtime1;
		mtime = mtime1 = millis();
		while (mtime1 - mtime < 10)
		{
			decoder.tick();
			mtime1 = millis();
		}
	}

	if (command == "SH")
	{
		command = "";
		CatInterface.SetIf(value);
	}

	if ((command == "TX" || command == "TUNE") && (tx == 0))
	{
		tx = 1;
		if (command == "TUNE")
			CatInterface.Settx(1);
		else
			CatInterface.Settx(2);
		command = "";
	}

	if (tx == 1)
	{
		lv_state_t state1 = lv_obj_get_state(button[4]);
		lv_state_t state = lv_obj_get_state(button[0]);
		if (((state & LV_STATE_CHECKED) == 0) && ((state1 & LV_STATE_CHECKED) == 0))
		{
			tx = 0;
			CatInterface.Settx(0);
		}
	}

	int count_vfo = Enc_vfo.getCount();
	Enc_vfo.clearCount();
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

	int count = 0;
	if (button_selected != -1)
	{
		count = (int)decoder.getDirection(1);
		if (count)
			decoder.ClearEncoder(1);
	}

	if (count)
	{
		switch (button_selected)
		{
		case 1: // Volume
			CatInterface.Setag(count);
			break;
		case 2: // RF Gain
			CatInterface.Setrg(count);
			break;
		case 3:
				
			break;
		}
	}

	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_timer_handler(); // let the GUI do its work 
	xSemaphoreGive(GuiBinarySemaphore);
	int currMillis = millis();
	if (!gui_band.have_buttons() && (currMillis - lastEncoding1 > 2000))
	{
		CatInterface.Getag();
		CatInterface.Getrg();
		CatInterface.Requestinformation(2);
		lastEncoding1 = currMillis;
	}
}

void button_event_handler(lv_event_t * e)
	{

		lv_obj_t* obj = lv_event_get_target(e);
		lv_obj_t* label = lv_obj_get_child(obj, 0L);
		char* ptr = lv_label_get_text(label);

		for (int i = 0; i < 5; i++)
		{
			if (button[i] != obj)
			{
				lv_obj_clear_state(button[i], LV_STATE_CHECKED);
			}
			else
			{
				if (i != button_selected)
				{
					button_selected = i;
					switch (i)
					{
					case 0:
						command = String("TX"); 
						break;
					case 1:
						command = String("VOL");
						break;
					case 2:
						command = String("GAIN");
						break;
					case 3:
						command = String("AGC");
						break;
					case 4:
						command = String("TUNE");
						break;
					}
				}
				else
				{
					button_selected = -1;
					command = String("");
					lv_obj_clear_state(button[i], LV_STATE_CHECKED);
				}

			}
		}

	}

	void button_filter_event_handler(lv_event_t * e)
	{

		lv_obj_t* obj = lv_event_get_target(e);
		lv_obj_t* label = lv_obj_get_child(obj, 0L);
		char* ptr = lv_label_get_text(label);

		for (int i = 0; i < 8; i++)
		{
			if (button_filter[i] != obj)
			{
				lv_obj_clear_state(button_filter[i], LV_STATE_CHECKED);
			}
			else
			{
				switch (i)
				{
					case 0:
						command = String("SH");
						value = 500;
						break;
					case 1:
						command = String("SH");
						value = 1000;
						break;
					case 2:
						command = String("SH");
						value = 1500;
						break;
					case 3:
						command = String("SH");
						value = 2000;
						break;
					case 4:
						command = String("SH");
						value = 2500;
						break;
					case 5:
						command = String("SH");
						value = 3000;
						break;
					case 6:
						command = String("SH");
						value = 3500;
						break;
					case 7:
						command = String("SH");
						value = 4000;
						break;
				}
			}
		}

	}

	void create_tab_setting(lv_obj_t* tab, lv_group_t* button_group)
	{
		int				ibutton_x = 0, ibutton_y = 0;

		for (int i = 0; i < 5; i++)
		{
			button[i] = lv_btn_create(tab);
			lv_group_add_obj(button_group, button[i]);
			lv_obj_add_style(button[i], &style_btn, 0);
			lv_obj_add_event_cb(button[i], button_event_handler, LV_EVENT_CLICKED, NULL);
			lv_obj_align(button[i], LV_ALIGN_TOP_LEFT, ibutton_x * button_width_margin, ibutton_y * button_height_margin);
			lv_obj_add_flag(button[i], LV_OBJ_FLAG_CHECKABLE);
			lv_obj_set_size(button[i], button_width, button_height);

			lv_obj_t* lv_label = lv_label_create(button[i]);

			char str[20];
			switch (i)
			{
			case 0:
				strcpy(str, "TX");
				break; 
			case 1:
				strcpy(str, "Volume");
				command = String("VOL");
				button_selected = i;
				lv_obj_add_state(button[i], LV_STATE_CHECKED);
				break;
			case 2:
				strcpy(str, "Gain");
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

	void create_tab_filter(lv_obj_t* tab, lv_group_t* button_group)
	{
		int				ibutton_x = 0, ibutton_y = 0; 
		
		for (int i = 0; i < 8; i++)
		{
			button_filter[i] = lv_btn_create(tab);
			lv_group_add_obj(button_group, button_filter[i]);
			lv_obj_add_style(button_filter[i], &style_btn, 0);
			lv_obj_add_event_cb(button_filter[i], button_filter_event_handler, LV_EVENT_CLICKED, NULL);
			lv_obj_align(button_filter[i], LV_ALIGN_TOP_LEFT, ibutton_x * button_width_margin, ibutton_y * button_height_margin);
			lv_obj_add_flag(button_filter[i], LV_OBJ_FLAG_CHECKABLE);
			lv_obj_set_size(button_filter[i], button_width, button_height);

			lv_obj_t* lv_label = lv_label_create(button_filter[i]);
			
			char str[20];
			switch (i)
			{
			case 0:
				strcpy(str, "0.5 Khz");
				break;
			case 1:
				strcpy(str, "1 Khz");
				break;
			case 2:
				strcpy(str, "1.5 Khz");
				break;
			case 3:
				strcpy(str, "2 Khz");
				break;
			case 4:
				strcpy(str, "2.5 Khz");
				break;
			case 5:
				strcpy(str, "3 Khz");
				break;
			case 6:
				strcpy(str, "3.5 Khz");
				break;
			case 7:
				strcpy(str, "4 Khz");
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

	static void scroll_begin_event(lv_event_t* e)
	{
		/*Disable the scroll animations. Triggered when a tab button is clicked */
		if (lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN) {
			lv_anim_t* a = (lv_anim_t*)lv_event_get_param(e);
			if (a)  a->time = 0;
		}
	}
