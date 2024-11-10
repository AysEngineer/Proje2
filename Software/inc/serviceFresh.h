#ifndef SERVICE_FRESH_H
#define SERVICE_FRESH_H

#include "defines.h"
#include "systemTick.h"
#include "ntc.h"
#include "system.h"
#include "buzzer.h"
#include "serviceDisplay.h"
#include "serviceError.h"
#include "xWatchdog.h"

void serviceFreshInit();
void serviceFreshRun();

uint8_t getTempSet();
void setTempSet(uint8_t value);
bool isFreshOn();
void setFreshOn();


void onOffButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing);
void plusButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing);
void minusButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing);

bool isHeaterDrawerOn();

#endif // SERVICE_FRESH_H
