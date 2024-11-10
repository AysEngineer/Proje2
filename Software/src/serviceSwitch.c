#include "serviceSwitch.h"

#define SWITCH_STABIL_COUNTER			            	5

enum SwitchType { SWITCH_ON_OFF = 0, SWITCH_PLUS, SWITCH_MINUS, SWITCHES_COUNT };

typedef struct {
	uint16_t inputPin;
    bool isPressing;

    uint8_t pressCounter;
    uint8_t unPressCounter;
} Swtch;

typedef struct {
	Swtch switches[SWITCHES_COUNT];
} _ServiceSwitch;

typedef struct {
    SystemTick runNextTrigger;
} _ServiceSwitchTime;

_ServiceSwitch _service_swtch;
_ServiceSwitchTime _service_swtch_time;


void __reset_swtch(Swtch* swtch) {
    swtch->isPressing = false;
    swtch->pressCounter = 0;
    swtch->unPressCounter = 0;
}

bool __detect_switch_status(Swtch* swtch, bool currentStatus) {
	if ( currentStatus ) {
		swtch->unPressCounter = 0;
		if ( swtch->pressCounter >= SWITCH_STABIL_COUNTER ) {
			if ( swtch->isPressing == false ) {	//Kullanici butona basti
				swtch->isPressing = true;
			}
		}
		else swtch->pressCounter++;
	} else {
		swtch->pressCounter = 0;
		if ( swtch->unPressCounter >= SWITCH_STABIL_COUNTER ) {
			if ( swtch->isPressing ) {	//Kullanici butondan elini cekti
				swtch->isPressing = false;
			}
		}
		else swtch->unPressCounter++;
	}	
	return swtch->isPressing;
}


void serviceSwitchInit() {
	for ( uint8_t i = 0; i < SWITCHES_COUNT; i++ ) {
		__reset_swtch(&_service_swtch.switches[i]);
	}
	_service_swtch.switches[SWITCH_ON_OFF].inputPin = SW1;
	_service_swtch.switches[SWITCH_PLUS].inputPin = SW3;
	_service_swtch.switches[SWITCH_MINUS].inputPin = SW2;
	copyAndIncSystemTick(&_service_swtch_time.runNextTrigger, SECONDS(1));
}


uint8_t butdat;
uint8_t prvdat;
uint8_t presdb;
uint8_t prvbut;
uint8_t vdcnt;
uint8_t button;
uint8_t cmp60msval = 12;
uint8_t lvdcnt;

uint8_t _pressType = 0;
uint8_t _pressedButton = 0;
uint8_t _pressedDb = 0;
uint8_t _pressTime = 0;
void serviceSwitchRun() {
	if ( isSystemTickElapsed(_service_swtch_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_service_swtch_time.runNextTrigger, SERVICE_SWITCH_RUN_TIME_MILLI);
	xWatchdogUpdate(APP_WTD_SWITCH);

     _pressType = 0;
     _pressedButton = 0;
     _pressTime = 0;

	//button data ile yap bir onceki ile ayni degilse tus basimi durdur
	bsp_io_level_t _bsp_temp;
	butdat = 0xff;
	for ( uint8_t i = 0; i < SWITCHES_COUNT; i++ ) {
		R_IOPORT_PinRead(&g_ioport_ctrl, _service_swtch.switches[i].inputPin, &_bsp_temp);
		butdat &= ~(__detect_switch_status(&_service_swtch.switches[i], _bsp_temp == BSP_IO_LEVEL_LOW) << i);
	}

	if ( butdat == prvdat ) {
		vdcnt++;
		if ( button != butdat ) {			//Long button control
			if ( vdcnt == 10 ) {
				vdcnt = 0;
				lvdcnt = 0;

				prvbut = button;
				button = butdat;
				presdb = (butdat ^ prvbut) & prvbut;

                _pressType = 1;
                _pressedButton = button;
                _pressTime = 0;
                _pressedDb = presdb;
			}
		} else {	//long button pressed
			if ( vdcnt == 20 ) {				        //20*5=100ms
				vdcnt=0;
				if ( lvdcnt!= 255 ) {
					lvdcnt++;
                    _pressType = 2;
                    _pressedButton = button;
                    _pressedDb = presdb;
                    _pressTime = lvdcnt;
				}
			}
		}
	} else {
        if ( butdat == 0xff ) {
            if ( button != 0x00 ) {
                _pressType = 3;
                _pressTime = lvdcnt;
                _pressedButton = button;
                _pressedDb = presdb;
            }
            button = 0x00;
            prvbut = 0x00;
            presdb = 0x00;
        }
		vdcnt = 0;
		lvdcnt = 0;
		prvdat = butdat;
	}

    if ( _pressType != 0 && _pressedDb != 0 ) {
        if ( _pressedButton == 0xfe ) {
            onOffButtonClicked(_pressTime, _pressType != 3);
        } else if ( _pressedButton == 0xfd ) {
            plusButtonClicked(_pressTime, _pressType != 3);
        } else if ( _pressedButton == 0xfb ) {
            minusButtonClicked(_pressTime, _pressType != 3);
        }		
    }

}
