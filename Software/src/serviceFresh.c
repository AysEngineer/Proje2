#include "serviceFresh.h"

//The preset temp if the electric power is off to be 65C
//Yazilimda sadece olculen deger x2 ayarlanan derecede boyle bir durum yok
#define FRESH_POWER_ON_TEMP_SET			65

//	The setting temp to vary from 50 to 80C
#define FRESH_TEMP_SET_MIN				50
#define FRESH_TEMP_SET_MAX				80
//ntc den olculen deger x2 oldugu icin FRESH_TEMP_SET_MAX x 2
#define FRESH_TEMP_NTC_MAX              160

enum FreshDisplayMode { FD_MODE_NORMAL, FD_MODE_ERROR };
enum FreshDisplayMenu { FD_MENU_OFF, FD_MENU_IDLE, FD_MENU_SET, FD_MENU_ERROR };
enum WaterHeatStatus { HEAT_OFF, HEAT_ON };

typedef struct {
	enum FreshDisplayMode displayMode;
	enum FreshDisplayMenu displayMenu;
	//Mode guncellemeden onceki FreshDisplayMenu
	enum FreshDisplayMenu displayMenuBeforeModeUpdate;
	//Ayarlanan sicaklik(x2 DEGIL!!!)
	uint8_t tempSet;

	enum WaterHeatStatus heatStatus;

	//Hata durumunda eger tuslardan birine basilirsa ses iptal edilir
	bool errorSoundEnabled;
} _ServiceFresh;

typedef struct {
	SystemTick runNextTrigger;
	//7sn sonra idleMenu'den cikar sadece error modunda!!!
	SystemTick idleMenuWaitTime;	
	//7sn sonra setMenu'den cikar
	SystemTick setMenuWaitTime;
	//Set ekraninda ayar yapilirsa 1sn flashlama durur
	SystemTick tempSetFlashingWaitTime;
	//En fazla 1dk boyunca hata durumunda buzzer sesi verebilir.
	SystemTick maxErrorSoundTime;
} _ServiceFreshTime;


_ServiceFresh _service_fresh;
_ServiceFreshTime _service_fresh_time;

void __show_fresh_off_menu();
//Eger error modunda ise heater aktif edilmez
void __show_fresh_idle_menu(bool updateHeater);
void __show_fresh_set_menu();
void __show_fresh_error_menu();
void __set_temp_set_value(uint8_t value);

uint8_t getTempSet() {
	return _service_fresh.tempSet;
}

void setTempSet(uint8_t value) {
	if ( value < FRESH_TEMP_SET_MIN || value > FRESH_TEMP_SET_MAX )  {
		return;
	}
	_service_fresh.tempSet = value;
}

bool isFreshOn() {
	return _service_fresh.displayMenu != FD_MENU_OFF;
}

void setFreshOn() {
	if ( _service_fresh.displayMode == FD_MODE_NORMAL )
		_service_fresh.displayMenu = FD_MENU_IDLE;
}


void serviceFreshInit() {
	_service_fresh.displayMode = FD_MODE_NORMAL;
	_service_fresh.displayMenu = FD_MENU_OFF;
	_service_fresh.heatStatus = HEAT_OFF;
	_service_fresh.tempSet = FRESH_POWER_ON_TEMP_SET;
	_service_fresh.errorSoundEnabled = false;
	copyAndIncSystemTick(&_service_fresh_time.runNextTrigger, 100);
	copyAndIncSystemTick(&_service_fresh_time.maxErrorSoundTime, MINUTES(1));
}

void __update_fresh_mode_and_menu(bool hasError) {
	enum FreshDisplayMenu _current_menu = _service_fresh.displayMenu;
	//Normal mode'da ve hata tespit edildi
	if ( _service_fresh.displayMode == FD_MODE_NORMAL && hasError ) {
		_service_fresh.displayMenuBeforeModeUpdate = _current_menu;
		//displayMenu eger idle ise FD_MENU_ERROR olarak guncelle
		if ( _service_fresh.displayMenu == FD_MENU_OFF ||
		     _service_fresh.displayMenu == FD_MENU_IDLE ) {
			_service_fresh.displayMenu = FD_MENU_ERROR;
		}
		_service_fresh.displayMode = FD_MODE_ERROR;
	} else if ( _service_fresh.displayMode == FD_MODE_ERROR && hasError == false ) {
		//Normal modda FD_MODE_ERROR gecerli degil
		if ( _service_fresh.displayMenuBeforeModeUpdate != FD_MENU_ERROR ) {
			_service_fresh.displayMenu = _service_fresh.displayMenuBeforeModeUpdate;
		}
		_service_fresh.displayMenuBeforeModeUpdate = _current_menu;
		if ( _service_fresh.displayMenu == FD_MENU_ERROR ) {
			_service_fresh.displayMenu = FD_MENU_OFF;
		}
		_service_fresh.displayMode = FD_MODE_NORMAL;
	}
}

void __service_fresh_run_normal() {
	switch ( _service_fresh.displayMenu ) {
		case FD_MENU_OFF:
			__show_fresh_off_menu();
			break;
		case FD_MENU_IDLE:
			__show_fresh_idle_menu(true);
			break;
		case FD_MENU_SET:
			if ( isSystemTickElapsed(_service_fresh_time.setMenuWaitTime) ) {
				_service_fresh.displayMenu = FD_MENU_IDLE;
			} else {
				__show_fresh_set_menu();
			}
			break;
		case FD_MENU_ERROR:	//Buraya girmemesi gerekiyor!!!
			_service_fresh.displayMenu = FD_MENU_OFF;
			break;									
	}
}

void __service_fresh_run_error() {
	switch ( _service_fresh.displayMenu ) {
		case FD_MENU_OFF:
			_service_fresh.displayMenu = FD_MENU_ERROR;
			break;
		case FD_MENU_IDLE:
			if ( isSystemTickElapsed(_service_fresh_time.idleMenuWaitTime) ) {
				_service_fresh.displayMenu = FD_MENU_ERROR;
			} else {
				__show_fresh_idle_menu(false);
			}
			break;
		case FD_MENU_SET:
			if ( isSystemTickElapsed(_service_fresh_time.setMenuWaitTime) ) {
				_service_fresh.displayMenu = FD_MENU_IDLE;
			} else {
				__show_fresh_set_menu();
			}
			break;
		case FD_MENU_ERROR:
			__show_fresh_error_menu();
			break;
	}
	_service_fresh.heatStatus = HEAT_OFF;
}

uint8_t relayUpdateCounter = 0;
void serviceFreshRun() {
    //Extra guvenlik icin bu sekilde yazildi.
    if ( ++relayUpdateCounter >= 20 ) {
        relayUpdateCounter = 0;
        int16_t __ntc_temp = getNTCTemp();
        int16_t __tempSet = _service_fresh.tempSet * 2;
        if ( __ntc_temp >= __tempSet || __ntc_temp >= FRESH_TEMP_NTC_MAX || isNTCOK() == false ) {
            _service_fresh.heatStatus = HEAT_OFF;
        }
        if ( _service_fresh.heatStatus == HEAT_ON ) {
            HEATER_ON;
        } else {
            HEATER_OFF;
        }
    }
	//Service 50 ms bir calissin
	if ( isSystemTickElapsed(_service_fresh_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_service_fresh_time.runNextTrigger, SERVICE_FRESH_RUN_TIME_MILLI);
	xWatchdogUpdate(APP_WTD_FRESH);

	bool _isErrorDetected = getErrorStatus() != ERROR_NONE;
	__update_fresh_mode_and_menu(_isErrorDetected);


	switch ( _service_fresh.displayMode ) {
		case FD_MODE_NORMAL:
			__service_fresh_run_normal();
			break;
		case FD_MODE_ERROR:
			__service_fresh_run_error();
			break;
	}


	if ( _isErrorDetected ) {
		setDisplayIconWarning(DISPLAY_FLASHING, 500);
		if ( getDisplayIconStatus() && _service_fresh.errorSoundEnabled &&
		     isSystemTickElapsed(_service_fresh_time.maxErrorSoundTime) == false )
			buzzerMakeBeep();
	} else {
	    copyAndIncSystemTick(&_service_fresh_time.maxErrorSoundTime, MINUTES(1));
		_service_fresh.errorSoundEnabled = true;
		setDisplayIconWarning(DISPLAY_OFF, 0);
	}

 	//Yazilimda sadece olculen deger x2 ayarlanan derecede boyle bir durum yok
	int16_t _tempSet = _service_fresh.tempSet * 2;
	if ( _service_fresh.heatStatus == HEAT_ON ) {
		setDisplayIconOnOff(DISPLAY_FLASHING, 1000);
	} else {
		//iceride sicak su var mi?
		if ( _tempSet - 20 <= getNTCTemp() && isNTCOK() ) 	setDisplayIconOnOff(DISPLAY_ON, 0);
		else									            setDisplayIconOnOff(DISPLAY_OFF, 0);
	}
}

void __show_fresh_off_menu() {
	setDisplayIconDegree(DISPLAY_OFF, 0);
	setDisplayLeftDigitChar(DSP_CHAR_hypen);
	setDisplayRightDigitChar(DSP_CHAR_hypen);
	_service_fresh.heatStatus = HEAT_OFF;
}


void __show_fresh_idle_menu(bool updateHeater) {
	//Yazilimda olculen derece x2
	int16_t _ntc_temp = getNTCTemp();
	//Yazilimda sadece olculen deger x2 ayarlanan derecede boyle bir durum yok
	int16_t _tempSet = _service_fresh.tempSet * 2;
	if ( updateHeater ) {
		if ( _service_fresh.heatStatus == HEAT_OFF ) {
			//the heater will restart to work when the temperature reaches the temperature 10C lower than the set one
			//Olculen Derece normalin x2
			if ( _tempSet - 20 >= _ntc_temp ) {
				_service_fresh.heatStatus = HEAT_ON;
			}
		}
	}

	//Olculen Derece 2 katı oldugu icin displayde gostermek icin 2'e bolunur
	_ntc_temp /= 2;

	setDisplayIconDegree(DISPLAY_ON, 0);

	if ( _ntc_temp > 99 ) {
		setDisplayDigitsOn(99);
	} else if ( _ntc_temp >= 0 ) {
		setDisplayDigitsOn(_ntc_temp);
	} else if ( _ntc_temp > -10 ) {
	    _ntc_temp = _ntc_temp * -1;
		setDisplayDigitsOn(_ntc_temp);
		setDisplayLeftDigitChar(DSP_CHAR_hypen);
	} else {
		setDisplayDigitsOn(9);
		setDisplayLeftDigitChar(DSP_CHAR_hypen);		
	}
}

void __show_fresh_set_menu() {
	setDisplayIconDegree(DISPLAY_FLASHING, 1000);

	//Set ekraninda ayar yapilirsa 1sn flashlama durur
	if ( isSystemTickElapsed(_service_fresh_time.tempSetFlashingWaitTime) ) {
	    setDisplayDigitsOnWithFlashing(_service_fresh.tempSet, 1000);
	} else {
	    setDisplayDigitsOn(_service_fresh.tempSet);
	}
}


void __show_fresh_error_menu() {
	setDisplayIconDegree(DISPLAY_OFF, 0);
	setDisplayLeftDigitChar(DSP_CHAR_E);
	switch ( getErrorStatus() ) {
		case ERROR_NONE:	//Yazilimsal hata asla buraya girmemesi gerekiyor
			setDisplayRightDigitChar(DSP_CHAR_0);
			break;
		case ERROR_NO_NTC_SENSOR:
			setDisplayRightDigitChar(DSP_CHAR_1);
			break;
		case ERROR_HIGH_TEMP:
			setDisplayRightDigitChar(DSP_CHAR_2);
			break;
		case ERROR_NO_WATER:
			setDisplayRightDigitChar(DSP_CHAR_3);
			break;
		case ERROR_HEATER_IN_TROUBLE:
			setDisplayRightDigitChar(DSP_CHAR_4);
			break;	
		case ERROR_FLASH:
			setDisplayRightDigitChar(DSP_CHAR_5);
			break;			
	}
}



void __set_temp_set_value(uint8_t value) {
	if ( _service_fresh.displayMenu != FD_MENU_SET ) {
		return;
	}
	if ( value >= FRESH_TEMP_SET_MIN && value <= FRESH_TEMP_SET_MAX ) {
		_service_fresh.tempSet = value;
	}
	copyAndIncSystemTick(&_service_fresh_time.tempSetFlashingWaitTime, SECONDS(1));
}

//Kullanici butona basarak display kapayliysa acabilir
//Display aciksa 2.sn uzun basimda kapatabilir.
bool _lockOnOffButton = false;
void onOffButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing) {
	_service_fresh.errorSoundEnabled = false;
    if ( longPressTimeIn100ms == 0 && isPressing )      buzzerMakeBeep();
	if ( isPressing == false ) {
		_lockOnOffButton = false;
	}
	if ( isPressing == false || _lockOnOffButton == true ) {
		return;
	}

	if ( longPressTimeIn100ms == 0 && _service_fresh.displayMenu == FD_MENU_OFF ) {
		_service_fresh.displayMenu = FD_MENU_IDLE;
		_lockOnOffButton = true;
	} else if ( longPressTimeIn100ms == 20 && _service_fresh.displayMenu != FD_MENU_OFF &&
	            _service_fresh.displayMode != FD_MODE_ERROR) {
		_service_fresh.displayMenu = FD_MENU_OFF;
		_lockOnOffButton = true;
		buzzerMakeBeep();
	}
}

void __update_menu_min_plus_press() {
	if ( _service_fresh.displayMode == FD_MODE_NORMAL ) {
		switch ( _service_fresh.displayMenu ) {
			case FD_MENU_OFF:
				break;
			case FD_MENU_IDLE:
				copyAndIncSystemTick(&_service_fresh_time.tempSetFlashingWaitTime, SECONDS(1));
				_service_fresh.displayMenu = FD_MENU_SET;
				break;
			case FD_MENU_SET:
				break;
			case FD_MENU_ERROR:
				break;
		}
	} else if ( _service_fresh.displayMode == FD_MODE_ERROR ) {
		switch ( _service_fresh.displayMenu ) {
			case FD_MENU_OFF:
				break;
			case FD_MENU_IDLE:
				copyAndIncSystemTick(&_service_fresh_time.tempSetFlashingWaitTime, SECONDS(1));
				_service_fresh.displayMenu = FD_MENU_SET;
				break;
			case FD_MENU_SET:
				break;
			case FD_MENU_ERROR:
				_service_fresh.displayMenu = FD_MENU_IDLE;
				break;
		}
	}
	
}

void plusButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing) {
	_service_fresh.errorSoundEnabled = false;
    if ( longPressTimeIn100ms == 0 && isPressing )      buzzerMakeBeep();
	if ( isPressing == false) {
		return;
	}

	if ( longPressTimeIn100ms == 0 ) {
		if ( _service_fresh.displayMenu == FD_MENU_SET ) {
			__set_temp_set_value(_service_fresh.tempSet + 1);
		} else {
			__update_menu_min_plus_press();
		}
	} else if ( longPressTimeIn100ms >= 10 && longPressTimeIn100ms % 3 == 0 ) {  //Uzun basim
        __set_temp_set_value(_service_fresh.tempSet + 1);
	}
	copyAndIncSystemTick(&_service_fresh_time.idleMenuWaitTime, SECONDS(7));
	copyAndIncSystemTick(&_service_fresh_time.setMenuWaitTime, SECONDS(7));
}

void minusButtonClicked(uint16_t longPressTimeIn100ms, bool isPressing) {
	_service_fresh.errorSoundEnabled = false;
    if ( longPressTimeIn100ms == 0 && isPressing )      buzzerMakeBeep();
    if ( isPressing == false) {
        return;
    }

    if ( longPressTimeIn100ms == 0 ) {
        if ( _service_fresh.displayMenu == FD_MENU_SET ) {
            __set_temp_set_value(_service_fresh.tempSet - 1);
        } else {
			__update_menu_min_plus_press();
		}
    } else if ( longPressTimeIn100ms >= 10 && longPressTimeIn100ms % 3 == 0 ) {  //Uzun basim
        __set_temp_set_value(_service_fresh.tempSet - 1);
    }
	copyAndIncSystemTick(&_service_fresh_time.idleMenuWaitTime, SECONDS(7));
	copyAndIncSystemTick(&_service_fresh_time.setMenuWaitTime, SECONDS(7));
}

/*
 * Role aktif mi?
*/
bool isHeaterDrawerOn() {
	return _service_fresh.heatStatus == HEAT_ON;
}
