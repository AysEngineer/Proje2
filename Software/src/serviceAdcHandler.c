#include "serviceAdcHandler.h"

enum AdcStatus { ADCSTATUS_IDLE, ADC_STATUS_WAITING_RES };

typedef struct {
	enum AdcStatus status;
} _AdcHandler;

typedef struct {
	SystemTick runNextTrigger;
} _AdcHandlerTime;

_AdcHandler _adc_hander;
_AdcHandlerTime _adc_handler_time;


void serviceAdcHandlerInit() {
	_adc_hander.status = ADCSTATUS_IDLE;
	copyAndIncSystemTick(&_adc_handler_time.runNextTrigger, 100);
}
uint16_t channel1_conversion_result;
void serviceAdcHandlerRun() {
	//Service 50 ms bir calissin
	if ( isSystemTickElapsed(_adc_handler_time.runNextTrigger) == false ) {
		return;
	}
	copyAndIncSystemTick(&_adc_handler_time.runNextTrigger, SERVICE_ADC_RUN_TIME_MILLI);
	xWatchdogUpdate(APP_WTD_ADC);

    adc_status_t _status;
    _status.state = ADC_STATE_SCAN_IN_PROGRESS;

	fsp_err_t err = FSP_SUCCESS;
	switch ( _adc_hander.status ) {
		case ADCSTATUS_IDLE:
			R_ADC_ScanStart(&g_adc0_ctrl);
			_adc_hander.status = ADC_STATUS_WAITING_RES;
		break;
		case ADC_STATUS_WAITING_RES:
			R_ADC_StatusGet(&g_adc0_ctrl, &_status);
			if ( _status.state != ADC_STATE_SCAN_IN_PROGRESS ) {
				err = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_7, &channel1_conversion_result);
				if ( err == FSP_SUCCESS  )	insertNTCAdcRaw(channel1_conversion_result);
				_adc_hander.status = ADCSTATUS_IDLE;
			}
		break;
	}
}
