#ifndef SERVICE_DISPLAY_H
#define SERVICE_DISPLAY_H

#include "defines.h"
#include "systemTick.h"
#include "display_frs_m3.h"
#include "xWatchdog.h"

void serviceDisplayInit();
void serviceDisplayRun();

enum DigitChars {
    DIG_CHAR_A = 10,
    DIG_CHAR_b,
    DIG_CHAR_C,
    DIG_CHAR_D,
    DIG_CHAR_E,
    DIG_CHAR_F,
    DIG_CHAR_SPACE,
    DIG_CHAR_L,
    DIG_CHAR_UNDER_SCORE,
    DIG_CHAR_r,
    DIG_CHAR_d,
    DIG_CHAR_u,
    DIG_CHAR_h,
    DIG_CHAR_degree,
    DIG_CHAR_n,
    DIG_CHAR_HYPEN
};

void setDisplayIconDegree(enum DisplayStatus status, uint16_t flashingIntervalMs);
void setDisplayIconWarning(enum DisplayStatus status, uint16_t flashingIntervalMs);
void setDisplayIconOnOff(enum DisplayStatus status, uint16_t flashingIntervalMs);
void setDisplayLeftDigitChar(enum DisplayChars diplayChar);
void setDisplayRightDigitChar(enum DisplayChars diplayChar);
void setDisplayLeftDigitCharWithFlashing(enum DisplayChars diplayChar, uint16_t flashingIntervalMs);
void setDisplayRightDigitCharWithFlashing(enum DisplayChars diplayChar, uint16_t flashingIntervalMs);
void setDisplayDigitsOn(uint8_t value);
void setDisplayDigitsOff();
void setDisplayDigitsOnWithFlashing(uint8_t value, uint16_t flashingIntervalMs);


bool getDisplayIconStatus();

#endif // SERVICE_DISPLAY_H
