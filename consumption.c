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
/*!
*
* @brief Tank volume calculator
*
*/

void
air_consumption (void * p_arg)
{

  OS_ERR  err;
  (void)p_arg;    // NOTE: Silence compiler warning about unused param.
  uint16_t depth;
  uint16_t volume;
  
  depth = dal_get_depth();
  volume = dal_get_air_volume();
  
  for (;;)
  {
    if (depth == 0)
    {
      // No air is consumed.

    }
    else if (depth > 0)
    {
      // Air is consumed based on depth.
      //output of cL converted to mL adding to previous 
      volume = volume - 10*gas_rate_in_cl(depth); 
	  dal_set_air_volume(volume);
    }
    else
    {
      //You've got a problem if you got here.
      assert(0);
    }
    //2Hz calc rate
    OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}
