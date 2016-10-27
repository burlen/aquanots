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
#include  "scuba.h"
#include "data_access_layer.h"
#include "alarm.h"

/*!
*
* @brief Tank volume calculator
*
*/

#define CL_TO_ML (10)
#define SURFACE (0)
void
air_consumption (void * p_arg)
{

  OS_ERR  err;
  (void)p_arg;    // NOTE: Silence compiler warning about unused param.
  
    for (;;)
  {
    uint16_t depth;
    uint16_t volume;
    
    depth = dal_get_depth();
    volume = dal_get_air_volume();
    
    if (depth == SURFACE)
    {
      // No air is consumed.

    }
    else if (depth > SURFACE)
    {
      // Air is consumed based on depth.
      //output of cL converted to mL adding to previous 
      if (volume > CL_TO_ML*gas_rate_in_cl(depth))
      {
        volume = volume - CL_TO_ML*gas_rate_in_cl(depth); 
      }
      else
      {
        volume = 0;
      }    
	  dal_set_air_volume(volume);
    }
    else
    {
      //You've got a problem if you got here.
      assert(0);
    }
    
    uint16_t min_air_vol;
    //Get max 
    min_air_vol = gas_to_surface_in_cl(depth);
    
    if (volume > min_air_vol)
    {
      //Trigger high alarm
      OSFlagPost(&g_alarm_flags,
                 ALARM_HIGH,
                 OS_OPT_POST_FLAG_SET,
                 &err);
      assert(OS_ERR_NONE == err);
    }
        
    //2Hz calc rate
    OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}
