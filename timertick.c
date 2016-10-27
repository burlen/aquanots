#include  "cpu_cfg.h"
#include  "bsp_cfg.h"
#include  "assert.h"
#include  "stdio.h"

#include  <cpu_core.h>
#include  <os.h>
#include  <bsp_glcd.h>

#include  "os_app_hooks.h"
#include  "data_access_layer.h"

void
timer_tick (void * p_arg)
{
  
  OS_ERR	    err;
  
  for (;;)	
    {
      uint32_t tick;
      tick = dal_get_tick();
      ++tick;
      dal_set_tick(tick);

      //1Hz cycle
      OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}