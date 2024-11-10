#ifndef SYSTEM_TICK_H
#define SYSTEM_TICK_H

#include "defines.h"

#define SECONDS(x) (x * 1000L)
#define MINUTES(x) (x * 60000L)
#define HOURS(x)   (x * 3600000L)

typedef struct {
	uint32_t systemTickLow;
	uint32_t systemTickHigh;
} SystemTick;

void initSystemTick();
const SystemTick* getSystemTick();
void incSystemTick();
bool isSystemTickElapsed(SystemTick temp_tick);
void copySystemTick(SystemTick *temp_tick);
void incGivenTick(SystemTick *temp_tick, uint32_t tick);
void copyAndIncSystemTick(SystemTick *temp_tick, uint32_t tick);
void resetGivenTick(SystemTick *temp_tick);
bool isGivenTickReset(SystemTick temp_tick);

#endif // SYSTEM_TICK_H
