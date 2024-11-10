#include "buzzer.h"

uint16_t __buzzerRunTimeMilli = 0;
uint16_t __buzzerPinState = 0;

bool __isBuzzerDisabled;
SystemTick __buzzerDisableEndTime;

void buzzerInit() {
    BZ1_OFF;
    __isBuzzerDisabled = false;
    resetGivenTick(&__buzzerDisableEndTime);
}

void buzzerMakeBeep() {
    if ( __isBuzzerDisabled ) {
        if ( isSystemTickElapsed(__buzzerDisableEndTime) == false ) {
            return;
        }
        setBuzzerEnabled();
    }
    __buzzerRunTimeMilli = 400; //400 * 0.125us = 50ms  
}

void buzzerMakeLongBeep() {
    if ( __isBuzzerDisabled ) {
        if ( isSystemTickElapsed(__buzzerDisableEndTime) == false ) {
            return;
        }
        setBuzzerEnabled();
    }
    __buzzerRunTimeMilli = 2000; //400 * 0.125us = 50ms
}

void setBuzzerDisabledForSec(uint8_t sec) {
    __isBuzzerDisabled = true;
    copyAndIncSystemTick(&__buzzerDisableEndTime, SECONDS(sec));
}

void setBuzzerEnabled() {
    __isBuzzerDisabled = false;
}

//125us
void buzzerUpdate() {
    if ( __buzzerRunTimeMilli != 0 ) {
        if ( __buzzerPinState == 1 ) {
            BZ1_OFF;
            __buzzerPinState = 0;
            --__buzzerRunTimeMilli;
        } else {
            BZ1_ON;
            __buzzerPinState = 1;
        }
    } else
        BZ1_OFF;
}

