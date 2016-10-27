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
display_refresh (void * p_arg)
{
  
  OS_ERR	    err;
  
  //Initialize some stuff here
  
  for (;;)	
  {
    //Put your display work here
    

    
    //10Hz cycle - OS delay
      OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}