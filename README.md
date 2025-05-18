# This repo is archived
Current Arduino version has made this project not fitting anymore in the ESP32 also libraries have updated and are not matching with the code any more.
This repo is only for reference.


# ESP32 USB CAT Controler

A USB remote control for sdrberry or other CAT enabled SDR or VFO
It also for sdrberry controles bandpassfilter and lpf

ToDo:
- add i2c for LPF and BPF

Done:
- USB CAT interface support 

Please add/modify a file credentials.h to the source files for wifi
```
#ifndef _CREDENTIALS_H_			// Prevent double inclusion
#define _CREDENTIALS_H_

#define NUMBR_SSID  2
const char* _ssid[] = {"wifi-ssid1", "wifi-ssid2" };
const char* _password[] = {"password", "password"};

#endif
```

![sdrberry](https://github.com/paulh002/sdrberry/blob/master/IMG_20231015_131803.jpg)

[![Radioberry demo](https://img.youtube.com/vi/BMJiv3YGv-k/0.jpg)](https://youtu.be/PQ_Np5SfcxA)

# ESP32 Remote control for raspberry pi
![sdrberry](https://github.com/paulh002/sdrberry/blob/master/rb_tranceiver.jpg)