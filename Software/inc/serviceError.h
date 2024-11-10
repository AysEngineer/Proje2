#ifndef SERVICE_ERROR_H
#define SERVICE_ERROR_H

#include "defines.h"
#include "systemTick.h"
#include "ntc.h"
#include "serviceFlash.h"
#include "xWatchdog.h"

enum ErrorStatus { ERROR_NONE, ERROR_NO_NTC_SENSOR, ERROR_HIGH_TEMP, ERROR_NO_WATER, ERROR_HEATER_IN_TROUBLE, ERROR_FLASH };

void serviceErrorInit();
void serviceErrorRun();

enum ErrorStatus getErrorStatus();

extern bool isHeaterDrawerOn();

#endif // SERVICE_ERROR_H
