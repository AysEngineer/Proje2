#include "system.h"


void initDevice() {
    //============== Timer interrupt ==============//
    R_GPT_Open(&timer_isr_0_ctrl, &timer_isr_0_cfg);
    R_GPT_Start(&timer_isr_0_ctrl);
    //============== ##### ######## ==============//
    //=================== ADC ====================//
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    //=================== ### ====================//
    //=================== IWDT ===================//
    R_IWDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);
    //=================== ### ====================//

    //=================== FLASH ==================//
    RM_VEE_FLASH_Open(&g_vee0_ctrl, &g_vee0_cfg);
    //=================== ### ====================//
}

