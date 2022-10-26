#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <lvgl.h>
#include <string>
#include "FT891_CAT.h"
#include "Cat.h"
#include "network.h"
#include "Gui_band.h"

void Cat::begin()
{
	cat_message.begin(false, &cat_comm, false);
	f_rxtx = false;
	afgain = 0;
	frequency_a = 0;
}

bool Cat::checkCAT()
{
	bool ret_val = cat_message.CheckCAT();
/*
	if (digitalRead(TXRX_SWITCH))
	{
		if (cat_message.GetTX() == TX_CAT)
		{
			f_rxtx = true;
		}
		else
		{
			f_rxtx = false;
			cat_message.SetTX((uint8_t)TX_OFF);
		}
	}
	else
	{
		f_rxtx = true;
	}
*/
	return ret_val;
}

// Send FT command to sdrberry
void Cat::Setft(int ft)
{
	cat_message.SetFT(ft);
}

void Cat::Setag(int ag)
{
	afgain = cat_message.GetAG(false) + ag;
	//afgain += ag;
	if (afgain < 0)
		afgain = 0;
	if (afgain > 255)
		afgain = 255;
	cat_message.SetAG((uint8_t)afgain);
}

uint8_t Cat::Getag()
{
	afgain = cat_message.GetAG();
	return afgain;
}

long long Cat::Getfa()
{
	frequency_a = cat_message.GetFA();
	return frequency_a;
}

void Cat::Setrg(int rg)
{
	rfgain += rg;
	if (rfgain < 0)
		rfgain = 0;
	if (rfgain > 255)
		rfgain = 255;
	cat_message.SetRG((uint8_t)rfgain);
}

void Cat::Settx(int tx)
{
	cat_message.SetTX((uint8_t)tx);
}

uint8_t Cat::Getrg()
{
	rfgain = cat_message.GetRG();
	return rfgain;
}

void Cat::Requestinformation(int info)
{
	char str[20];

	sprintf(str, "GT%d;", info);
	cat_comm.Send(str);
}

void Comm::Send(std::string s)
{
	Serial.print(s.c_str());
	char str[BUF_LEN];
	sprintf(str, "Send --> %s \r\n", s.c_str());
	DebugServer.write(0, (const uint8_t *)str, strlen(str));
}

void Comm::Read(char c, std::string& s)
{
	char szBuffer[BUF_LEN];
	
	memset(szBuffer, 0, BUF_LEN);
	Serial.readBytesUntil(c, szBuffer, sizeof(szBuffer));
	for (int i = 0; i < strlen(szBuffer); i++)
		s.push_back(szBuffer[i]);

	char str[30];
	sprintf(str, "Receive --> %s \r\n", s.c_str());
	DebugServer.write(0, (const uint8_t*)str, strlen(str));
}

bool Comm::available()
{
	if (Serial.available())
		return true;
	else
		return false;
}

void Comm::SendInformation(int info)
{

}

void Comm::setband(int band)
{
	gui_band.set_button(band);
}

void Comm::Processinformation(std::string s)
{
	char c = s.at(0);
	switch (c)
	{
	case '0':
		break;
	case '1':
		break;
	case '2':
		{
			std::string s1;

			gui_band.reset_button();
			std::string::iterator it = s.begin();
			it++; it++;
			if (it != s.end())
			{
				do {
					if (*it == ',')
					{
						gui_band.add_button(s1);
						s1.clear();
					}
					else
						s1.push_back(*it);
					it++;
				} while (it != s.end());
				gui_band.add_button(s1);
			}
			break;
		}
	}
}

void Cat::SetBnd(int bnd)
{
	cat_message.SetBand(bnd);
}

void Cat::SetIf(int if_filter)
{
	cat_message.SetSH(0,if_filter);
}

int32_t Cat::GetIf()
{
	return cat_message.GetSH();
}