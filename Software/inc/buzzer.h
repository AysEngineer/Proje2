#ifndef BUZZER_H
#define BUZZER_H
    
#include "defines.h"
#include "system.h"
#include "systemTick.h"

void buzzerInit();
void buzzerMakeBeep();
void buzzerMakeLongBeep();
void buzzerUpdate();

void setBuzzerDisabledForSec(uint8_t sec);
void setBuzzerEnabled();

#endif // BUZZER_H
