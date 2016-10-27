#include  <assert.h>
#include  <cpu_core.h>
#include  <os.h>
#include  <bsp_glcd.h>
#include  <stdio.h>

#include "switch_handler.h"
#include  "pushbutton.h"


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
#ifdef DISPLAY_SWITCH
      sprintf(&msg[0], "SW1: %4u", sw1_counter);
      BSP_GraphLCD_String(0, (const char *)&msg);
#endif
    }
}

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
#ifdef DISPLAY_SWITCH
      sprintf(&msg[0], "SW2: %4u", sw2_counter);
      BSP_GraphLCD_String(1, (const char *)&msg);
#endif
    }
}
