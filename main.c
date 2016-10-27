/** \file main.c
*
* @brief Embedded Software Boot Camp
*
* @par
* COPYRIGHT NOTICE: (C) 2014 Barr Group, LLC.
* All rights reserved.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "cpu_cfg.h"
#include  "bsp_cfg.h"
#include  "assert.h"

#include  <cpu_core.h>
#include  <os.h>
#include  <bsp_glcd.h>
#include  <stdio.h>

#include  "bsp.h"
#include  "bsp_int_vect_tbl.h"
#include  "bsp_led.h"
#include  "os_app_hooks.h"
#include  "pushbutton.h"
#include  "adc.h"
#include  "alarm.h"
#include  "switch_handler.h"
#include  "gui.h"

#include "consumption.h"
#include "heartbeat.h"
#include "timertick.h"
#include "display.h"
// TODO -- this is just for testing and is not needed
#include "data_access_layer.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

void led_toggle(uint8_t led, uint16_t period);

// Relative Task Priorities (0 = highest; 15 = idle task)
#define  STARTUP_TASK_PRIO      10   // Every 500 ms, in a timed loop.
#define  LED5_PRIO              12   // Every 500 ms, in a timed loop.
#define  LED7_PRIO              12   // Every 333 ms, in a timed loop.
#define  DEBOUNCE_PRIO          12   // 
#define  SW1_REACT_PRIO        12   // 
#define  SW2_REACT_PRIO        12   // 
#define  ADC_PRIO              12   // 
#define  ALARM_PRIO            14

#define  HEART_PRIO             9       //Needs to be modified
#define  CONS_PRIO              9       //Needs to be modified
#define  TIMER_PRIO             10      //Needs to be modified
#define  DISPLAY_PRIO           10      //Needs to be modified

// Allocate Task Stacks
#define  TASK_STACK_SIZE      128
#define  MAX_ADC_Q_SIZE       128

static CPU_STK  g_startup_task_stack[TASK_STACK_SIZE];
static CPU_STK  g_led5_stack[TASK_STACK_SIZE];
static CPU_STK  g_led7_stack[TASK_STACK_SIZE];
static CPU_STK  g_debounce_stack[TASK_STACK_SIZE];
static CPU_STK  g_sw1_react_stack[TASK_STACK_SIZE];
static CPU_STK  g_sw2_react_stack[TASK_STACK_SIZE];
static CPU_STK  g_adc_stack[TASK_STACK_SIZE];
static CPU_STK  g_alarm_stack[TASK_STACK_SIZE];
static CPU_STK  g_heartbeat_stack[TASK_STACK_SIZE]; 
static CPU_STK  g_consumption_stack[TASK_STACK_SIZE]; 
static CPU_STK  g_timer_stack[TASK_STACK_SIZE];
static CPU_STK  g_display_stack[TASK_STACK_SIZE];

// Allocate Task Control Blocks
static OS_TCB   g_startup_task_tcb;
static OS_TCB   g_led5_tcb;
static OS_TCB   g_led7_tcb;
static OS_TCB   g_debounce_tcb;
static OS_TCB   g_sw1_react_tcb;
static OS_TCB   g_sw2_react_tcb;
static OS_TCB   g_adc_tcb;
static OS_TCB   g_alarm_tcb;
static OS_TCB   g_heartbeat_tcb;
static OS_TCB   g_consumption_tcb;
static OS_TCB   g_timer_tcb;
static OS_TCB   g_display_tcb;

// Mutex
static OS_MUTEX g_led_mutex;
// Semaphore
OS_SEM g_sw1_sem;
OS_SEM g_sw2_sem;
// flags
OS_FLAG_GRP g_alarm_flags;


/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


void
startup_task (void * p_arg)
{
    OS_ERR  err;


    (void)p_arg;    // NOTE: Silence compiler warning about unused param.
    // Perform hardware initializations that should be after multitasking.
    BSP_Init();
    CPU_Init();
    Mem_Init();
    BSP_GraphLCD_SetFont(GLYPH_FONT_8_BY_8);
    dal_reset();
    


    gui_display_msg_on_line(6,"Hello1");
    gui_display_status_msg("Hello2");
    gui_display_brand();
    gui_display_depth_rate();
    
    OSTaskCreate((OS_TCB     *)&g_heartbeat_tcb,
                 (CPU_CHAR   *)"Heartbeat Task",
                 (OS_TASK_PTR ) heartbeat_LED,
                 (void       *) 0,
                 (OS_PRIO     ) HEART_PRIO,
                 (CPU_STK    *)&g_heartbeat_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
    
        OSTaskCreate((OS_TCB     *)&g_consumption_tcb,
                 (CPU_CHAR   *)"Consumption Task",
                 (OS_TASK_PTR ) air_consumption,
                 (void       *) 0,
                 (OS_PRIO     ) CONS_PRIO,
                 (CPU_STK    *)&g_consumption_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
    
    OSTaskCreate((OS_TCB     *)&g_timer_tcb,
                 (CPU_CHAR   *)"Timer Task",
                 (OS_TASK_PTR ) timer_tick,
                 (void       *) 0,
                 (OS_PRIO     ) TIMER_PRIO,
                 (CPU_STK    *)&g_timer_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
    
    OSTaskCreate((OS_TCB     *)&g_display_tcb,
                 (CPU_CHAR   *)"Display Task",
                 (OS_TASK_PTR ) display_refresh,
                 (void       *) 0,
                 (OS_PRIO     ) DISPLAY_PRIO,
                 (CPU_STK    *)&g_display_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
    //End of Robert's section
    
       OSTaskCreate((OS_TCB     *)&g_debounce_tcb,
                 (CPU_CHAR   *)"Debounce Task",
                 (OS_TASK_PTR ) debounce_task,
                 (void       *) 0,
                 (OS_PRIO     ) DEBOUNCE_PRIO,
                 (CPU_STK    *)&g_debounce_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSTaskCreate((OS_TCB     *)&g_sw1_react_tcb,
                 (CPU_CHAR   *)"SW1 React Task",
                 (OS_TASK_PTR ) sw1_react_task,
                 (void       *) 0,
                 (OS_PRIO     ) SW1_REACT_PRIO,
                 (CPU_STK    *)&g_sw1_react_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSTaskCreate((OS_TCB     *)&g_sw2_react_tcb,
                 (CPU_CHAR   *)"SW2 React Task",
                 (OS_TASK_PTR ) sw2_react_task,
                 (void       *) 0,
                 (OS_PRIO     ) SW2_REACT_PRIO,
                 (CPU_STK    *)&g_sw2_react_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSTaskCreate((OS_TCB     *)&g_adc_tcb,
                 (CPU_CHAR   *)"ADC Task",
                 (OS_TASK_PTR ) adc_task,
                 (void       *) 0,
                 (OS_PRIO     ) ADC_PRIO,
                 (CPU_STK    *)&g_adc_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSTaskCreate((OS_TCB     *)&g_alarm_tcb,
                 (CPU_CHAR   *)"Alarm Task",
                 (OS_TASK_PTR ) alarm_task,
                 (void       *) 0,
                 (OS_PRIO     ) ALARM_PRIO,
                 (CPU_STK    *)&g_alarm_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSMutexCreate(&g_led_mutex, "LED Mutex", &err);
    assert(OS_ERR_NONE == err);
    
    OSSemCreate(&g_sw1_sem, "SW1 Sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSSemCreate(&g_sw2_sem, "SW2 Sem", 0, &err);
    assert(OS_ERR_NONE == err);
    
    //OSQCreate(&adcQ, "ADC Q", MAX_ADC_Q_SIZE, &err);
    //assert(OS_ERR_NONE == err);
    
    OSTaskDel((OS_TCB     *)&g_startup_task_tcb, (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
}
/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : (1) It is assumed that your code will call main() once you have performed all necessary
*                   initialization.
*********************************************************************************************************
*/

void  main (void)
{
    OS_ERR  err;

    CPU_IntDis();                                               /* Disable all interrupts.                              */

    BSP_IntVectSet(27, (CPU_FNCT_VOID)OSCtxSwISR);              /* Setup kernel context switch                          */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */
    assert(OS_ERR_NONE == err);

    // Install application-specific OS hooks.
    App_OS_SetAllHooks();

    // Create the LED task.
    OSTaskCreate((OS_TCB     *)&g_startup_task_tcb,
                 (CPU_CHAR   *)"Startup Task",
                 (OS_TASK_PTR ) startup_task,
                 (void       *) 0,
                 (OS_PRIO     ) STARTUP_TASK_PRIO,
                 (CPU_STK    *)&g_startup_task_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);
    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    // We should never get here.
    assert(0);
}

