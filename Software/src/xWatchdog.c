#include "xWatchdog.h"

bool __serviceAdcRunning = false;
bool __serviceFreshRunning = false;
bool __serviceDisplayRunnning = false;
bool __serviceSwitchRunning = false;
bool __serviceErrorRunning = false;
bool __serviceErrorOneSec = false;
bool __serviceFlashRunning = false;

void xWatchdogUpdate(enum AppWatchdog appWatchdog) {
	switch ( appWatchdog ) {
		case APP_WTD_ADC:
			__serviceAdcRunning = true;
			break;
		case APP_WTD_FRESH:
			__serviceFreshRunning = true;
			break;
		case APP_WTD_DISPLAY:
			__serviceDisplayRunnning = true;
			break;
		case APP_WTD_SWITCH:
			__serviceSwitchRunning = true;
			break;
		case APP_WTD_ERROR:
			__serviceErrorRunning = true;
			break;
		case APP_WTD_ERROR_ONE_SEC:
			__serviceErrorOneSec = true;
			break;
		case APP_WTD_FLASH:
			__serviceFlashRunning = true;
			break;			
	}
}

void xWatchdogRefresh() {
	if ( __serviceAdcRunning && __serviceFreshRunning && 
		 __serviceDisplayRunnning && __serviceSwitchRunning && 
		 __serviceErrorRunning   && __serviceErrorOneSec && __serviceFlashRunning ) {
		//Update watchdog
	    R_IWDT_Refresh(&g_wdt0_ctrl);
		__serviceAdcRunning = false;
		__serviceFreshRunning = false;
		__serviceDisplayRunnning = false;
		__serviceSwitchRunning = false;
		__serviceErrorRunning = false;
		__serviceErrorOneSec = false;
		__serviceFlashRunning = false;
	}
}
