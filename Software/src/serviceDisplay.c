#include "serviceDisplay.h"

enum ServiceDisplayState { SERVICE_DISPLAY_COM1, SERVICE_DISPLAY_COM2 };

typedef struct {
    enum ServiceDisplayState displayState;
    DisplayIcons displayIcons;
    DisplayDigits displayDigits;
} _ServiceDisplay;

typedef struct {
    SystemTick runNextTrigger;
} _ServiceDisplayTime;

_ServiceDisplay _service_display;
_ServiceDisplayTime _service_display_time;

void __reset_all_flashing() {
    if ( _service_display.displayIcons.degreeStatus.status == DISPLAY_FLASHING ) {
        copyAndIncSystemTick(&_service_display.displayIcons.degreeStatus.flashingNextToggleTime, _service_display.displayIcons.degreeStatus.flashingIntervalMs);
        _service_display.displayIcons.degreeStatus.isOn = true;
    }
    if ( _service_display.displayIcons.warningStatus.status == DISPLAY_FLASHING ) {
        copyAndIncSystemTick(&_service_display.displayIcons.warningStatus.flashingNextToggleTime, _service_display.displayIcons.warningStatus.flashingIntervalMs);
        _service_display.displayIcons.warningStatus.isOn = true;
    }
    if ( _service_display.displayIcons.onOffStatus.status == DISPLAY_FLASHING ) {
        copyAndIncSystemTick(&_service_display.displayIcons.onOffStatus.flashingNextToggleTime, _service_display.displayIcons.onOffStatus.flashingIntervalMs);
        _service_display.displayIcons.onOffStatus.isOn = true;
    }
    if ( _service_display.displayDigits.digitH.status == DISPLAY_FLASHING ) {
        copyAndIncSystemTick(&_service_display.displayDigits.digitH.flashingNextToggleTime, _service_display.displayDigits.digitH.flashingIntervalMs);
        _service_display.displayDigits.digitH.isOn = true;
    }
    if ( _service_display.displayDigits.digitL.status == DISPLAY_FLASHING ) {
        copyAndIncSystemTick(&_service_display.displayDigits.digitL.flashingNextToggleTime, _service_display.displayDigits.digitL.flashingIntervalMs);
        _service_display.displayDigits.digitL.isOn = true;
    }    
}

//Belirtilen Digiti kapat
void __reset_display_digit(DisplayDigit* displayDigit) {
    displayDigit->status = DISPLAY_OFF;
    displayDigit->flashingIntervalMs = 0;
    resetGivenTick(&displayDigit->flashingNextToggleTime);
}

//Belirtilen Iconu kapat
void __reset_display_icon(DisplayIcon* displayIcon) {
    displayIcon->status = DISPLAY_OFF;
    displayIcon->flashingIntervalMs = 0;
    resetGivenTick(&displayIcon->flashingNextToggleTime);
}


//Digiti flashlatir
void __set_display_digit_with_flashing(DisplayDigit* displayDigit, uint16_t flashingIntervalMs) {
    if ( displayDigit->status == DISPLAY_FLASHING && displayDigit->flashingIntervalMs == flashingIntervalMs ) {
        return;
    }
    displayDigit->status = DISPLAY_FLASHING;
    displayDigit->flashingIntervalMs = flashingIntervalMs;
    copyAndIncSystemTick(&displayDigit->flashingNextToggleTime, flashingIntervalMs);
    __reset_all_flashing();
}

//Iconu flashlatir
void __set_display_icon_with_flashing(DisplayIcon* displayIcon, uint16_t flashingIntervalMs) {
    if ( displayIcon->status == DISPLAY_FLASHING && displayIcon->flashingIntervalMs == flashingIntervalMs ) {
        return;
    }    
    displayIcon->status = DISPLAY_FLASHING;
    displayIcon->flashingIntervalMs = flashingIntervalMs;
    copyAndIncSystemTick(&displayIcon->flashingNextToggleTime, flashingIntervalMs);
    __reset_all_flashing();
}

//Digiti guncelle
void __set_display_digit(DisplayDigit* displayDigit, enum DisplayStatus status, uint8_t value, uint16_t flashingIntervalMs) {
    if ( status == DISPLAY_FLASHING ) {
        __set_display_digit_with_flashing(displayDigit, flashingIntervalMs);
    } else {
        __reset_display_digit(displayDigit);
        displayDigit->status = status;
    }
    displayDigit->value = value;
}

//Iconu guncelle
void __set_display_icon(DisplayIcon* displayIcon, enum DisplayStatus status, uint16_t flashingIntervalMs) {
    if ( status == DISPLAY_FLASHING ) {
        __set_display_icon_with_flashing(displayIcon, flashingIntervalMs);
    } else {
        __reset_display_icon(displayIcon);
        displayIcon->status = status;
    }
}

//Derece iconunu guncelle
void setDisplayIconDegree(enum DisplayStatus status, uint16_t flashingIntervalMs) {
    __set_display_icon(&_service_display.displayIcons.degreeStatus, status, flashingIntervalMs);
}

bool getDisplayIconStatus() {
    return _service_display.displayIcons.warningStatus.isOn;
}

//Uyari iconunu guncelle
void setDisplayIconWarning(enum DisplayStatus status, uint16_t flashingIntervalMs) {
    __set_display_icon(&_service_display.displayIcons.warningStatus, status, flashingIntervalMs);
}

//OnOff iconunu guncelle
void setDisplayIconOnOff(enum DisplayStatus status, uint16_t flashingIntervalMs) {
    __set_display_icon(&_service_display.displayIcons.onOffStatus, status, flashingIntervalMs);
}

void setDisplayLeftDigitChar(enum DisplayChars diplayChar) {
    __set_display_digit(&_service_display.displayDigits.digitH, DISPLAY_ON, diplayChar, 0);
}

void setDisplayRightDigitChar(enum DisplayChars diplayChar) {
    __set_display_digit(&_service_display.displayDigits.digitL, DISPLAY_ON, diplayChar, 0);
}

void setDisplayLeftDigitCharWithFlashing(enum DisplayChars diplayChar, uint16_t flashingIntervalMs) {
    __set_display_digit(&_service_display.displayDigits.digitH, DISPLAY_FLASHING, diplayChar, flashingIntervalMs);
}

void setDisplayRightDigitCharWithFlashing(enum DisplayChars diplayChar, uint16_t flashingIntervalMs) {
    __set_display_digit(&_service_display.displayDigits.digitL, DISPLAY_FLASHING, diplayChar, flashingIntervalMs);
}


//Digitleri ac
void setDisplayDigitsOn(uint8_t value) {
    __set_display_digit(&_service_display.displayDigits.digitH, DISPLAY_ON, value / 10, 0);
    __set_display_digit(&_service_display.displayDigits.digitL, DISPLAY_ON, value % 10, 0);
}

//Digitleri kapat
void setDisplayDigitsOff() {
    __set_display_digit(&_service_display.displayDigits.digitH, DISPLAY_OFF, 0, 0);
    __set_display_digit(&_service_display.displayDigits.digitL, DISPLAY_OFF, 0, 0);
}

//Digitleri flashla
void setDisplayDigitsOnWithFlashing(uint8_t value, uint16_t flashingIntervalMs) {
    if ( value > 99 )   value = 99;
    __set_display_digit(&_service_display.displayDigits.digitH, DISPLAY_FLASHING, value / 10, flashingIntervalMs);
    __set_display_digit(&_service_display.displayDigits.digitL, DISPLAY_FLASHING, value % 10, flashingIntervalMs);
}

void serviceDisplayInit() {
    resetGivenTick(&_service_display_time.runNextTrigger);
    _service_display.displayState = SERVICE_DISPLAY_COM1;
    __reset_display_icon(&_service_display.displayIcons.degreeStatus);
    __reset_display_icon(&_service_display.displayIcons.warningStatus);
    __reset_display_icon(&_service_display.displayIcons.onOffStatus);

    setDisplayIconDegree(DISPLAY_OFF, 0);
    setDisplayIconWarning(DISPLAY_OFF, 0);    
    setDisplayIconOnOff(DISPLAY_OFF, 0);
    setDisplayDigitsOff();
}

void __update_icon(DisplayIcon* displayIcon) {
    switch ( displayIcon->status ) {
        case DISPLAY_OFF:
            displayIcon->isOn = false;
            break;
        case DISPLAY_ON:
            displayIcon->isOn = true;
            break;
        case DISPLAY_FLASHING:
            if ( isSystemTickElapsed(displayIcon->flashingNextToggleTime) ) {
                if ( displayIcon->isOn )       displayIcon->isOn = false;
                else                           displayIcon->isOn = true;
                copyAndIncSystemTick(&displayIcon->flashingNextToggleTime, displayIcon->flashingIntervalMs);                
            }
            break;
    }
}

void __update_digit(DisplayDigit* displayDigit) {
    switch ( displayDigit->status ) {
        case DISPLAY_OFF:
            displayDigit->isOn = false;
            break;
        case DISPLAY_ON:
            displayDigit->isOn = true;
            break;
        case DISPLAY_FLASHING:
            if ( isSystemTickElapsed(displayDigit->flashingNextToggleTime) ) {
                if ( displayDigit->isOn )       displayDigit->isOn = false;
                else                            displayDigit->isOn = true;
                copyAndIncSystemTick(&displayDigit->flashingNextToggleTime, displayDigit->flashingIntervalMs);
            }
            break;
    }
}

void serviceDisplayRun() {
	if ( isSystemTickElapsed(_service_display_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_service_display_time.runNextTrigger, SERVICE_DISPLAY_RUN_TIME_MILLI);
	xWatchdogUpdate(APP_WTD_DISPLAY);

    __update_icon(&_service_display.displayIcons.degreeStatus);
    __update_icon(&_service_display.displayIcons.warningStatus);
    __update_icon(&_service_display.displayIcons.onOffStatus);

    __update_digit(&_service_display.displayDigits.digitH);
    __update_digit(&_service_display.displayDigits.digitL);

    switch ( _service_display.displayState ) {
        case SERVICE_DISPLAY_COM1:
            setDisplayFrsM3(COM_1, &_service_display.displayDigits.digitH, &_service_display.displayIcons);
            _service_display.displayState = SERVICE_DISPLAY_COM2;
            break;
        case SERVICE_DISPLAY_COM2:
            setDisplayFrsM3(COM_2, &_service_display.displayDigits.digitL, &_service_display.displayIcons);
            _service_display.displayState = SERVICE_DISPLAY_COM1;
            break;
    }
	
}
