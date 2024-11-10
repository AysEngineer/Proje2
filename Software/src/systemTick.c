#include "systemTick.h"

SystemTick _system_tick;

/*
 * System Tickini init et.
*/
void initSystemTick() {
	_system_tick.systemTickLow = 0;
	_system_tick.systemTickHigh = 0;
}

/*
 * System tickini al
*/
const SystemTick* getSystemTick() {
	return &_system_tick;
}

/*
 * System tickini 1 artirir
 * systemTickLow 0 olmasi durumunda integer max degerini gecmistir ve
 * systemTickHigh degerini 1 artirir
*/
void incSystemTick() {
    _system_tick.systemTickLow++;
    if ( _system_tick.systemTickLow == 0 ) {
        _system_tick.systemTickHigh++;
    }
}

/*
 * Verilen temp_tick systemin tickine esitse veya gecmisse true dondurur.
*/
bool isSystemTickElapsed(SystemTick temp_tick) {
    if ( _system_tick.systemTickHigh == temp_tick.systemTickHigh ) {
        if ( _system_tick.systemTickLow >= temp_tick.systemTickLow ) {
            return true;
        } else {
            return false;
        }
    } else if ( _system_tick.systemTickHigh > temp_tick.systemTickHigh ) {
	  	return true;
	} else {
        return false;
    }
}

/*
 * System Tickini verilen tick'e kopyalar.
*/
void copySystemTick(SystemTick *temp_tick) {
	temp_tick->systemTickLow = _system_tick.systemTickLow;
	temp_tick->systemTickHigh = _system_tick.systemTickHigh;
}

/*
 * Verilen ticki arttir.
*/
void incGivenTick(SystemTick *temp_tick, uint32_t tick) {
    if ( tick == 0 ) {
        return;
    }
    uint32_t temp_low_tick = temp_tick->systemTickLow;
	temp_tick->systemTickLow += tick;
    if ( temp_low_tick >= temp_tick->systemTickLow ) {
        temp_tick->systemTickHigh++;
    }
}

/*
 * Verilen ticki system tickiyle guncelleyip verilen tick kadar arttirir.
*/
void copyAndIncSystemTick(SystemTick *temp_tick, uint32_t tick) {
    copySystemTick(temp_tick);
    incGivenTick(temp_tick, tick);
}


/*
 * Verilen SystemTicki reset durumuna getirir.
*/
void resetGivenTick(SystemTick *temp_tick) {
	temp_tick->systemTickLow = 0;
	temp_tick->systemTickHigh = 0;
}

/*
 * Parametre olarak verilen SystemTickin reset olma durumunu dondurur.
*/
bool isGivenTickReset(SystemTick temp_tick) {
	if ( temp_tick.systemTickLow != 0 ) 		return false;
	if ( temp_tick.systemTickHigh != 0 ) 		return false;
    
	return true;
}
