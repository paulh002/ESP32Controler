/*
 Name:		ESPControler.ino
 Created:	12/1/2022 5:36:30 PM
 Author:	paulh
*/
#include <EEPROM.h>
#include <Wire.h>
#include <WiFi.h>
#include <lvgl.h>
#include "network.h"
#include "measurement.h"
#include "credentials.h"
#include "gui.h"
#include "_EEPROMAnything.h"

SemaphoreHandle_t swrBinarySemaphore = NULL;
SemaphoreHandle_t GuiBinarySemaphore = NULL;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	
	//pinMode(S_METER, ANALOG);
	pinMode(FWD_METER, ANALOG);
	pinMode(REV_METER, ANALOG);

	GuiBinarySemaphore = xSemaphoreCreateMutex();
	if (GuiBinarySemaphore == NULL) {
		Serial.println("Error creating the GuiBinarySemaphore");
	}
	swrBinarySemaphore = xSemaphoreCreateMutex();
	if (swrBinarySemaphore == NULL) {
		Serial.println("Error creating the swrBinarySemaphore");
	}
	net_queue = xQueueCreate(1, sizeof(uint8_t));
	if (net_queue == NULL) {
		Serial.println("Error creating the queue");
	}
	
	LoadEEPROM();
	start_measurement();
	init_gui();
	delay(10);

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

// the loop function runs over and over again until power down or reset


void loop() {

		if (net_queue)
		{ // Check if we need to start or stop wifi
			uint8_t message;
			if (xQueueReceive(net_queue, &message, 0))
			{
				if (message == 0)
				{
					// stopwifi
					vfo_network.network_stop();
				}
				if (message == 1)
				{
					// startwifi
					vfo_network.begin(TRXNET_SERVER);
				}
			}
		}
		vfo_network.network_loop();
		delay(1);
}


void LoadEEPROM()
{
	uint8_t coldstart;

	EEPROM.begin(sizeof(var_t) + sizeof(uint8_t));

	coldstart = EEPROM.read(0);               // Grab the coldstart byte indicator in EEPROM for
	// comparison with the COLDSTART_REFERENCE

	Serial.println(String("Coldstart : ") + String(coldstart));

	// Initialize all memories if first upload or if COLDSTART_REF has been modified
	// either through PSWR_A.h or through Menu functions

	if (coldstart == COLDSTART_REF-1)
	{
		init_measurement(true);  // upgrade

		EEPROM_readAnything(1, R);               // Read the stored data
		return;
	}

	if (coldstart != COLDSTART_REF)
	{
		init_measurement(false);
		EEPROM.write(0, COLDSTART_REF);          // COLDSTART_REF in first byte indicates all initialized
		EEPROM_writeAnything(1, R);              // Write default settings into EEPROM
		EEPROM.commit();
	}
	else                                      // EEPROM contains stored data, retrieve the data
	{
		EEPROM_readAnything(1, R);               // Read the stored data
	}
}

void SaveEEPROM()
{
	EEPROM.write(0, COLDSTART_REF);          // COLDSTART_REF in first byte indicates all initialized
	EEPROM_writeAnything(1, R);              // Write default settings into  
	EEPROM.commit();
}
