#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_flash_lp.h"
#include "r_flash_api.h"
#include "rm_vee_flash.h"
#include "r_iwdt.h"
#include "r_wdt_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"


/* Flash on Flash LP Instance. */
extern const flash_instance_t g_flash0;

/** Access the Flash LP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_lp_instance_ctrl_t g_flash0_ctrl;
extern const flash_cfg_t g_flash0_cfg;

#ifndef rm_vee_flash_callback
void rm_vee_flash_callback(flash_callback_args_t *p_args);
#endif
extern const rm_vee_instance_t g_vee0;
extern rm_vee_flash_instance_ctrl_t g_vee0_ctrl;
extern const rm_vee_cfg_t g_vee0_cfg;

/** Callback used by VEE Instance. */
#ifndef vee_callback
void vee_callback(rm_vee_callback_args_t *p_args);
#endif


/** WDT on IWDT Instance. */
extern const wdt_instance_t g_wdt0;

/** Access the IWDT instance using these structures when calling API functions directly (::p_api is not used). */
extern iwdt_instance_ctrl_t g_wdt0_ctrl;
extern const wdt_cfg_t g_wdt0_cfg;

#ifndef iwdtCallback
void iwdtCallback(wdt_callback_args_t *p_args);
#endif


/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_channel_cfg_t g_adc0_channel_cfg;

/** Timer on GPT Instance. */
extern const timer_instance_t timer_isr_0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t timer_isr_0_ctrl;
extern const timer_cfg_t timer_isr_0_cfg;

#ifndef timer_isr_0_cb
void timer_isr_0_cb(timer_callback_args_t *p_args);
#endif

void setServicesRunnable();

#ifndef true
#define true			1
#endif

#ifndef false
#define false			0
#endif


#ifndef bool
#define bool            uint8_t
#endif


#endif // DEFINES_H
