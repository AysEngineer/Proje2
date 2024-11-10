#ifndef SYSTEM_H
#define SYSTEM_H

#include "defines.h"


void initDevice();

#define BZ1_ON                  R_IOPORT_PinWrite(&g_ioport_ctrl, BUZZER, BSP_IO_LEVEL_HIGH)
#define BZ1_OFF                 R_IOPORT_PinWrite(&g_ioport_ctrl, BUZZER, BSP_IO_LEVEL_LOW)


#define HEATER_ON              R_IOPORT_PinWrite(&g_ioport_ctrl, RLY, BSP_IO_LEVEL_HIGH)
#define HEATER_OFF             R_IOPORT_PinWrite(&g_ioport_ctrl, RLY, BSP_IO_LEVEL_LOW)
#endif // SYSTEM_H
