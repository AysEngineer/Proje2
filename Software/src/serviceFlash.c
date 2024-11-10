#include "serviceFlash.h"

#define ID_FLASH_REGION    (1U)

#define FLASH_READ_MAX_ERROR_COUNT  10
#define FLASH_REGION_KEY            0xAC

enum FlashRunMode { FLASH_RUN_UPDATE_SYSTEM, FLASH_IDLE, FLASH_WRITE, FLASH_READ };

volatile bool callback_called = true;
void vee_callback(rm_vee_callback_args_t * p_args) {
    callback_called = true;
    FSP_PARAMETER_NOT_USED(p_args);
}

typedef struct {
    uint8_t key;
    uint8_t tempSet;
    bool isFreshOn;
} _FlashFrame;

typedef struct {
    enum FlashRunMode runMode;
    uint8_t flashErrorCounter;

    //Systemin guncel datasi
    _FlashFrame preSystemFrame;

} _ServiceFlash;

typedef struct {
    SystemTick runNextTrigger;
    SystemTick writeLockTime;
} _ServiceFlashTime;


_ServiceFlash _service_flash;
_ServiceFlashTime _service_flash_time;


/* Record ID to use for storing pressure data. */
#define ID_PRESSURE    (1U)
_FlashFrame *_flashFrame_data;
//Flashta bulunan data
_FlashFrame currentFlashFrame;
uint32_t length;


fsp_err_t __write_flash() {
    callback_called = false;
    return RM_VEE_FLASH_RecordWrite(&g_vee0_ctrl, ID_PRESSURE, (uint8_t *) &currentFlashFrame, sizeof(_FlashFrame));
}

fsp_err_t __read_flash() {
    return RM_VEE_FLASH_RecordPtrGet(&g_vee0_ctrl, ID_PRESSURE, (uint8_t **) &_flashFrame_data, &length);
}



void serviceFlashInit() {
    __update_current_flash_frame(0, false);
    _service_flash.runMode = FLASH_RUN_UPDATE_SYSTEM;
    _service_flash.flashErrorCounter = 0;
   copyAndIncSystemTick(& _service_flash_time.runNextTrigger, 250);
}

void __update_pre_sytem_frame() {
    _service_flash.preSystemFrame.tempSet = getTempSet();
    _service_flash.preSystemFrame.isFreshOn = isFreshOn();    
}

void __update_current_flash_frame(uint8_t tempSet, bool isFreshOn) {
    currentFlashFrame.key = FLASH_REGION_KEY;
    currentFlashFrame.tempSet = tempSet;
    currentFlashFrame.isFreshOn = isFreshOn;
}

void __inc_flash_error() {
    if (  _service_flash.flashErrorCounter < FLASH_READ_MAX_ERROR_COUNT )   _service_flash.flashErrorCounter++;
}

bool isFlashOK() {
    return _service_flash.flashErrorCounter < FLASH_READ_MAX_ERROR_COUNT;
}


void serviceFlashRun() {
	//Service 50 ms bir calissin
	if ( isSystemTickElapsed(_service_flash_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_service_flash_time.runNextTrigger, SERVICE_FLASH_RUN_TIME_MILLI);
    xWatchdogUpdate(APP_WTD_FLASH);

    if ( isFlashOK() == false || callback_called == false  ) {
        return;
    }

    switch ( _service_flash.runMode ) {
        case FLASH_RUN_UPDATE_SYSTEM:
            _flashFrame_data->key = 0x00;
            if ( __read_flash() == FSP_SUCCESS ) {
                if ( _flashFrame_data->key == FLASH_REGION_KEY ) {
                    if ( _flashFrame_data->isFreshOn )  setFreshOn();
                    setTempSet(_flashFrame_data->tempSet);
                    __update_current_flash_frame(_flashFrame_data->tempSet, _flashFrame_data->isFreshOn);
                }
                _service_flash.runMode = FLASH_IDLE;
            } else {
                __inc_flash_error();
            }
            __update_pre_sytem_frame();
            return;
        case FLASH_IDLE:
            break;
        case FLASH_WRITE:
            if ( isSystemTickElapsed(_service_flash_time.writeLockTime) ) {
                if ( currentFlashFrame.tempSet != _service_flash.preSystemFrame.tempSet || 
                     currentFlashFrame.isFreshOn != _service_flash.preSystemFrame.isFreshOn ) {
                        __update_current_flash_frame(_service_flash.preSystemFrame.tempSet, _service_flash.preSystemFrame.isFreshOn);
                        if ( __write_flash(currentFlashFrame) == FSP_SUCCESS ) {
                            _service_flash.runMode = FLASH_IDLE;
                        } else {
                            __inc_flash_error();
                        }
                }
            }
            break;
        case FLASH_READ:
            break;
    }

    if ( _service_flash.preSystemFrame.tempSet != getTempSet() || _service_flash.preSystemFrame.isFreshOn != isFreshOn() ) {
        _service_flash.runMode = FLASH_WRITE;
        copyAndIncSystemTick(&_service_flash_time.writeLockTime, SECONDS(10));
    }
    __update_pre_sytem_frame();
}
