#include  <assert.h>
#include  <cpu_core.h>
#include  <os.h>
#include  <bsp_glcd.h>
#include  <stdio.h>

#include "data_access_layer.h"
#include "switch_handler.h"
#include  "pushbutton.h"

static uint8_t unitsToggle = DAL_UNITS_METRIC;

// Handles switch 1 press logic
// Add air to the tank
void sw1_react_task(void *p_arg)
{
    OS_ERR  err;
#ifdef DISPLAY_SWITCH
    char msg[32];
#endif
    uint32_t sw1_counter=0;

    (void)p_arg;    // NOTE: Silence compiler warning about unused param.
    
#ifdef DISPLAY_SWITCH
    sprintf(&msg[0], "SW1: %4u", sw1_counter);
    BSP_GraphLCD_String(0, (const char *)&msg);
#endif
    
    for (;;)
    {
      OSSemPend(&g_sw1_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
      assert(OS_ERR_NONE == err);
      sw1_counter++;
      if(dal_get_depth() == 0)
      {
        dal_add_air_volume_in_Millilitres(0);
      }
#ifdef DISPLAY_SWITCH
      sprintf(&msg[0], "SW1: %4u", sw1_counter);
      BSP_GraphLCD_String(0, (const char *)&msg);
#endif
    }
}

// Handles switch 2 press logic
// Toggles units selection
void sw2_react_task(void *p_arg)
{
    OS_ERR  err;
#ifdef DISPLAY_SWITCH
    char msg[32];
#endif
    uint32_t sw2_counter=0;

    (void)p_arg;    // NOTE: Silence compiler warning about unused param.

#ifdef DISPLAY_SWITCH
    sprintf(&msg[0], "SW2: %4u", sw2_counter);
    BSP_GraphLCD_String(1, (const char *)&msg);
#endif
    for (;;)
    {
      OSSemPend(&g_sw2_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
      assert(OS_ERR_NONE == err);
      sw2_counter++;
      unitsToggle = (unitsToggle==DAL_UNITS_METRIC)?DAL_UNITS_US:DAL_UNITS_METRIC;
      dal_set_units_toggle((uint8_t) unitsToggle);
#ifdef DISPLAY_SWITCH
      sprintf(&msg[0], "SW2: %4u", sw2_counter);
      sprintf(&msg[0], "SW2: %s", (unitsToggle==DAL_UNITS_US)?"DAL_UNITS_US":"DAL_UNITS_METRIC");
      //sprintf(&msg[0], "SW2: %4u", sw2_counter);
      BSP_GraphLCD_String(1, (const char *)&msg);
#endif
    }
}
