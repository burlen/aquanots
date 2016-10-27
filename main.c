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

// Allocate Task Control Blocks
static OS_TCB   g_startup_task_tcb;
static OS_TCB   g_led5_tcb;
static OS_TCB   g_led7_tcb;
static OS_TCB   g_debounce_tcb;
static OS_TCB   g_sw1_react_tcb;
static OS_TCB   g_sw2_react_tcb;
static OS_TCB   g_adc_tcb;
static OS_TCB   g_alarm_tcb;

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

void led_toggle(uint8_t led, uint16_t period)
{
    OS_ERR  err;
      OSMutexPend(&g_led_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
      assert(OS_ERR_NONE == err);

      BSP_LED_Toggle(led);
      OSMutexPost(&g_led_mutex, OS_OPT_POST_NONE, &err);
      OSTimeDlyHMSM(0, 0, 0, period, OS_OPT_TIME_HMSM_STRICT, &err);
}

void
led5_task (void * p_arg)
{
    //OS_ERR  err;


    (void)p_arg;    // NOTE: Silence compiler warning about unused param.

    for (;;)
    {
        // Flash LED at 1 Hz.
      led_toggle(5, 500);
//            OSMutexPend(&g_led_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
//            assert(OS_ERR_NONE == err);
//
//	    BSP_LED_Toggle(5);
//            OSMutexPost(&g_led_mutex, OS_OPT_POST_NONE, &err);
//	    OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

void
led7_task (void * p_arg)
{
    //OS_ERR  err;


    (void)p_arg;    // NOTE: Silence compiler warning about unused param.

    for (;;)
    {
        // Flash LED at 3 Hz.
            led_toggle(6, 333);
//            OSMutexPend(&g_led_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
//            assert(OS_ERR_NONE == err);
//	    BSP_LED_Toggle(6);
//            OSMutexPost(&g_led_mutex, OS_OPT_POST_NONE, &err);
//	    OSTimeDlyHMSM(0, 0, 0, 333, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

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
    
    OSTaskCreate((OS_TCB     *)&g_led5_tcb,
                 (CPU_CHAR   *)"LED5 Task",
                 (OS_TASK_PTR ) led5_task,
                 (void       *) 0,
                 (OS_PRIO     ) LED5_PRIO,
                 (CPU_STK    *)&g_led5_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

    OSTaskCreate((OS_TCB     *)&g_led7_tcb,
                 (CPU_CHAR   *)"LED7 Task",
                 (OS_TASK_PTR ) led7_task,
                 (void       *) 0,
                 (OS_PRIO     ) LED7_PRIO,
                 (CPU_STK    *)&g_led7_stack[0],
                 (CPU_STK_SIZE) TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TASK_STACK_SIZE ,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) 0,
                 (OS_ERR     *)&err);
    assert(OS_ERR_NONE == err);

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

