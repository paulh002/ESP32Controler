#include "BandFilter.h"

PCF8574 PCF_BPF0(0x38);
PCF8574 PCF_BPF1(0x39);

#define SDL     4
#define SDA     26

const int bpf080	= 0x16;
const int bpf040	= 0x19;
const int bpf020	= 0x25;
const int bpf017	= 0x15;
const int bpf0trg	= 0x15;
const int bpf180	= 0x85;
const int bpf140	= 0x85;
const int bpf120	= 0x85;
const int bpf117	= 0x86;
const int bpf1trg	= 0x89;

const int bpf180tx	= 0x45;
const int bpf140tx	= 0x45;
const int bpf120tx	= 0x45;
const int bpf117tx	= 0x46;
const int bpf1trgtx	= 0x49;


void BandFilter::Setup()
{
    int i = 0;
    do
    {
        if (!PCF_BPF0.begin(SDA, SDL, bpf080))
        {
            Serial.println("PCF_BPF0 could not initialize...");
        }
        if (!PCF_BPF0.isConnected())
        {
            Serial.println("=> not connected");
        }
        else 
        {
            Serial.println("PCF_BPF0 connected...");
        }
        if (!PCF_BPF1.begin())
        {
            Serial.println("PCF_BPF1 could not initialize...");
        }
        if (!PCF_BPF1.isConnected())
        {
            Serial.println("=> not connected");
        }
        else
        {
            Serial.println("PCF_BPF1 connected...");
            break;
        }
        i++;
    } while (i < 10);
    PCF_BPF0.write8(bpf017);
    PCF_BPF1.write8(bpf117);
}

void BandFilter::setBand(std::string band)
{

}


