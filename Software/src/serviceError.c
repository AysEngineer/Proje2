#include "serviceError.h"

#define NO_WATER_OBSERVER_BUFFER_SIZE	20

//Water temperature is over 95. yazilimda olculen dereceler x2
#define NTC_MAX_TEMPERATURE				180 //  Water temperature is over 90  // orginal 190;

struct _ErrorNoWaterObserver {
	bool errorDetected;
    uint8_t writeIndex;
    int16_t buffer[NO_WATER_OBSERVER_BUFFER_SIZE];
    bool arrayFilledFirstTime;
} _error_no_water_observer;

typedef struct {
	enum ErrorStatus errorStatus;
	int16_t lastNTCTemp;
} _ServiceError;


typedef struct {
	SystemTick runNextTrigger;
	//10dk boyunca eger heater aciksa ve degerece artmiyorsa ERROR_HEATER_IN_TROUBLE hatasi
	SystemTick heaterInTroubleEndTime;
	SystemTick oneSecCallback;
} _ServiceErrorTime;

_ServiceError _service_error;
_ServiceErrorTime _service_error_time;

enum ErrorStatus getErrorStatus() {
	return _service_error.errorStatus;
}

void __error_one_sec_callback() {
    //Heater kapali olsada check!
	if ( isNTCOK() == false || _error_no_water_observer.errorDetected
		 /*|| isHeaterDrawerOn() == false*/ ) {
		_error_no_water_observer.arrayFilledFirstTime = false;
		_error_no_water_observer.writeIndex = 0;
		return;
	}
	if ( _error_no_water_observer.arrayFilledFirstTime == false ) {
		if ( _error_no_water_observer.writeIndex >= NO_WATER_OBSERVER_BUFFER_SIZE ) {
			_error_no_water_observer.writeIndex = NO_WATER_OBSERVER_BUFFER_SIZE;
			_error_no_water_observer.arrayFilledFirstTime = true;
		} else {
			_error_no_water_observer.buffer[_error_no_water_observer.writeIndex++] = getNTCTemp();
		}
	} else {
		for ( uint8_t i = 0; i < NO_WATER_OBSERVER_BUFFER_SIZE - 1; i++ ) {
			_error_no_water_observer.buffer[i] = _error_no_water_observer.buffer[i + 1];
		}
		_error_no_water_observer.buffer[NO_WATER_OBSERVER_BUFFER_SIZE - 1] = getNTCTemp();
	}

	if ( _error_no_water_observer.writeIndex > NO_WATER_OBSERVER_BUFFER_SIZE ) {
		_error_no_water_observer.writeIndex = NO_WATER_OBSERVER_BUFFER_SIZE;
	}

	for ( uint8_t i = 1; i < _error_no_water_observer.writeIndex; i++ ) {
		//When the thermostat is open, the value read from ntc increases very quickly. 
		//At this point can you share the value increase rate with Okida?  
		//Apply : 5 degree increase in 20 sec
		//Yazilimda olculen dereceler x2 oldugu icin 
		if ( _error_no_water_observer.buffer[i] >= _error_no_water_observer.buffer[0] + 10 ) {
			_error_no_water_observer.errorDetected = true;
			break;
		}
	}

}

void serviceErrorInit() {
	copyAndIncSystemTick(&_service_error_time.runNextTrigger, 100);
	copyAndIncSystemTick(&_service_error_time.oneSecCallback, SECONDS(1));
	copyAndIncSystemTick(&_service_error_time.heaterInTroubleEndTime, MINUTES(10));
    _error_no_water_observer.arrayFilledFirstTime = false;
    _error_no_water_observer.writeIndex = 0;
	_service_error.errorStatus = ERROR_NONE;
}

void serviceErrorRun() {
	if ( isSystemTickElapsed(_service_error_time.oneSecCallback) ) {
		__error_one_sec_callback();
		copyAndIncSystemTick(&_service_error_time.oneSecCallback, SERVICE_ERROR_ONE_SEC_TIME_MILLI);
		xWatchdogUpdate(APP_WTD_ERROR_ONE_SEC);
	}

	//Service 500 ms bir calissin
	if ( isSystemTickElapsed(_service_error_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_service_error_time.runNextTrigger, SERVICE_ERROR_RUN_TIME_MILLI);
	xWatchdogUpdate(APP_WTD_ERROR);

	int16_t _ntcTemp = getNTCTemp();

	if ( isNTCOK() == false ) {
		_service_error.errorStatus = ERROR_NO_NTC_SENSOR;
	} else if ( _error_no_water_observer.errorDetected ) {
		_service_error.errorStatus = ERROR_NO_WATER;
	} else if ( _ntcTemp >= NTC_MAX_TEMPERATURE ) {
		_service_error.errorStatus = ERROR_HIGH_TEMP;
	} else if ( isSystemTickElapsed(_service_error_time.heaterInTroubleEndTime) ) {
		_service_error.errorStatus = ERROR_HEATER_IN_TROUBLE;
	} else if ( isFlashOK() == false ) {
		_service_error.errorStatus = ERROR_FLASH;
	}


	/*
     * Heater aciksa ve sicakik artiyorsa sureyi otele
     * Hatada olmama durumunda heater kapaliysa veya ntc hatasinda
     * */
    if ( ( isHeaterDrawerOn() && _ntcTemp > _service_error.lastNTCTemp ) ||
         ( _service_error.errorStatus != ERROR_HEATER_IN_TROUBLE && ( isNTCOK() == false || isHeaterDrawerOn() == false ) )) {
        copyAndIncSystemTick(&_service_error_time.heaterInTroubleEndTime, MINUTES(10));
    }

    _service_error.lastNTCTemp = _ntcTemp;
}
