#ifndef DISPLAY_FRS_M3_H
#define DISPLAY_FRS_M3_H

#include "defines.h"
#include "systemTick.h"

enum DisplayStatus { DISPLAY_OFF, DISPLAY_ON, DISPLAY_FLASHING };
enum DisplayCOM { COM_1, COM_2 };

typedef struct {
    bool isOn;
    enum DisplayStatus status;
    uint16_t flashingIntervalMs;
    SystemTick flashingNextToggleTime;
} DisplayIcon;

typedef struct {
    bool isOn;
    uint8_t value;
    enum DisplayStatus status;
    uint16_t flashingIntervalMs;
    SystemTick flashingNextToggleTime;
} DisplayDigit;

typedef struct {
    DisplayIcon degreeStatus;
    DisplayIcon warningStatus;
    DisplayIcon onOffStatus;
} DisplayIcons;

typedef struct {
    DisplayDigit digitH;
    DisplayDigit digitL;
} DisplayDigits;

enum DisplayChars {
    DSP_CHAR_0 = 0,
    DSP_CHAR_1,
    DSP_CHAR_2,
    DSP_CHAR_3,
    DSP_CHAR_4,
    DSP_CHAR_5,
    DSP_CHAR_6,
    DSP_CHAR_7,
    DSP_CHAR_8,
    DSP_CHAR_9,
    DSP_CHAR_A,
    DSP_CHAR_b,
    DSP_CHAR_C,
    DSP_CHAR_D,
    DSP_CHAR_E,
    DSP_CHAR_F,
    DSP_CHAR_SPACE,
    DSP_CHAR_L,
    DSP_CHAR_UNDER_SCORE,
    DSP_CHAR_r,
    DSP_CHAR_d,
    DSP_CHAR_u,
    DSP_CHAR_h,
    DSP_CHAR_degree,
    DSP_CHAR_n,
    DSP_CHAR_hypen,
    DSP_SUPPORTED_CHARS
};

void setDisplayFrsM3(enum DisplayCOM com, DisplayDigit* displayDigit, const DisplayIcons* displayIcons);


#endif // DISPLAY_FRS_M3_H
