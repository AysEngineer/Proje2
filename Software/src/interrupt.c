#include "interrupt.h"

uint8_t _125usSystemTickCounter = 0;
//1ms interrupt
void timer_isr_0_cb(timer_callback_args_t *p_args) {
    buzzerUpdate();
    if ( ++_125usSystemTickCounter == 8 ) { //1ms elapsed
        _125usSystemTickCounter = 0;
        incSystemTick();
        xWatchdogRefresh();
        setServicesRunnable();
    }
}
