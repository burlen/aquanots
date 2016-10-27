#include  "cpu_cfg.h"
#include  "bsp_cfg.h"
#include  "assert.h"
#include  "stdio.h"

#include  <cpu_core.h>
#include  <os.h>
#include  <bsp_glcd.h>

#include  "bsp.h"
#include  "bsp_int_vect_tbl.h"
#include  "bsp_led.h"
#include  "os_app_hooks.h"

/*!
*
* @brief LED Flasher Task
*
*/
void
heartbeat (void * p_arg)
{
    OS_ERR  err;


    (void)p_arg;    // NOTE: Silence compiler warning about unused param.

    for (;;)
    {
        // Flash LED at 3 Hz.
	protectedLED_Toggle(6);
	OSTimeDlyHMSM(0, 0, 0, 167, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}