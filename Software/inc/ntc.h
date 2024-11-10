#ifndef NTC_H
#define NTC_H

#include "defines.h"
#include "util.h"
#include "systemTick.h"

void ntcInit();

void insertNTCAdcRaw(uint16_t adc_raw);

//NTC sensorunde okunabilecek deger var mi?
bool isNTCReadyToRead();
//NTC sensoru calisiyormu?
bool isNTCOK();
//NTC sensorundeki degeri al
int16_t getNTCTemp();
//NTC sensorundeki adc degerini al
uint16_t getNTCFilteredRaw();

#endif // NTC_H
