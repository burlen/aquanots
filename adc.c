/** \file adc.c
*
* @brief ADC Driver
*
* @par
* COPYRIGHT NOTICE: (c) 2014 Barr Group, LLC.
* All rights reserved.
*/

#include <assert.h>
#include <stdint.h>				
#include <stdio.h>

#include "os.h"
#include "iorx63n.h"

#include "adc.h"
#include  <bsp_glcd.h>
#include "alarm.h"
#include "data_access_layer.h"
#include "scuba.h"

// Message Queue for ISR->Task Communication
static OS_Q  g_adc_q;


#define ADC_SOURCE_VR1      2

#define ADC_INTERRUPT_AFTER_SCAN    0x10
#define ADC_START                   0x80

#define BIT(n)              (1 << (n))


// NOTE: Refer to p. 1,698 to 1,727  of Processor_UsersManual_Hardware.pdf
typedef struct
{
  uint8_t           control;                      // Offset 0x00
  uint8_t  const    _unused1[3];
  uint16_t          channel_select0;              // Offset 0x04
  uint16_t          channel_select1;              // Offset 0x06
  uint16_t          value_addition_mode_select0;  // Offset 0x08
  uint16_t          value_addition_mode_select1;  // Offset 0x0A
  uint8_t           value_addition_count_select;  // Offset 0x0C
  uint8_t  const    _unused2;
  uint16_t          control_extended;             // Offset 0x0E
  uint8_t           start_trigger_select;         // Offset 0x10
  uint8_t  const    _unused3;
  uint16_t          extended_input_control;       // Offset 0x12
  uint8_t  const    _unused4[6];
  uint16_t          temperature_sensor_data;      // Offset 0x1A
  uint16_t          internal_reference_data;      // Offset 0x1C
  uint8_t  const    _unused5[2];
  uint16_t          data[21];                     // Offset 0x20
} adc_t;

adc_t volatile * const p_adc = (adc_t *) 0x00089000;
#define adc     (*p_adc)


/*!
* @brief Configure the ADC hardware to read Potentiometer VR1 and interrupt.
*/
static
void adc_config (void)
{
    /* Protection off */
    SYSTEM.PRCR.WORD = 0xA503u;            
    
    /* Cancel the S12AD module clock stop mode */
    MSTP_S12AD = 0;

    /* Protection on */
    SYSTEM.PRCR.WORD = 0xA500u;        

    // Select the 12-bit ADC. In the HW course, this detail is in BSP_Init().
    SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;

    /* Use the AN000 (Potentiometer) pin 
       as an I/O for peripheral functions */
    PORT4.PMR.BYTE = 0x01;

    // Enable A/D interrupts at an appropriate priority, as instructed.
    uint8_t * p_IER = (uint8_t *)0x0008720C;
    uint8_t * p_IPR = (uint8_t *)0x00087366;

    *p_IPR = 9;                         // Set interrupt priority.
    *p_IER |= 0x40;                     // Unmask A/D interrupt.

	// Configure the A/D to perform a single scan and interrupt.
    adc.control = ADC_INTERRUPT_AFTER_SCAN;
    adc.channel_select0 = BIT(ADC_SOURCE_VR1);
}

/*!
*
* @brief: ADC Driver Task
*/
void
adc_task (void * p_arg)
{
    OS_ERR     err;
    OS_MSG_SIZE msgSize;
    void *msgPtr;
    uint16_t adcData=0;
#ifdef DISPLAY_ADC
    char msg[32];
#endif
    //char alarm='-';
    int16_t depth_rate;

    (void)p_arg;    // NOTE: Silence compiler warning about unused param.

    // Create message queue.
    // NOTE: It's safe to do this here, because the ISR is synchornized to us.
    OSQCreate(&g_adc_q, "ADC Queue", 64, &err);
    assert(OS_ERR_NONE == err);	

    // Configure ADC hardware to read Potentiometer VR1 and interrupt.
    adc_config();
	
#ifdef DISPLAY_ADC
    sprintf(&msg[0], "ADC: %4u", adcData);
    BSP_GraphLCD_String(5, (const char *)&msg);
#endif

    for (;;)	
    {
        // Wait 125 ms.
        OSTimeDlyHMSM(0, 0, 0, 125, OS_OPT_TIME_HMSM_STRICT, &err);
        assert(OS_ERR_NONE == err);
											
        // Trigger ADC conversion.
        adc.control |= ADC_START;
			
        // TODO: Wait for message from ISR, then format and display on LCD.;
        msgPtr = OSQPend(&g_adc_q, 0, OS_OPT_PEND_BLOCKING, &msgSize, NULL, &err);
        assert(OS_ERR_NONE == err);
        adcData = (uint16_t)msgPtr;
        
        depth_rate = ADC2RATE(adcData);
        dal_set_depth_rate(depth_rate);
      
        if(depth_rate > DANGEROUS_ASCENT_RATE_M_MIN)
        {
          OSFlagPost(&g_alarm_flags, ALARM_MEDIUM, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
        else
        {
          OSFlagPost(&g_alarm_flags, ALARM_NONE, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
#if 0
        if(adcData == 0)
        { // No Alarm
          alarm= '-';
          OSFlagPost(&g_alarm_flags, ALARM_NONE, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
        else if((adcData >= 1) && (adcData <= 499))
        { // Low priority alarm
          alarm= 'L';
          OSFlagPost(&g_alarm_flags, ALARM_LOW, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
        else if((adcData >= 500) && (adcData <= 523))
        { // Med priority alarm
          alarm= 'M';
          OSFlagPost(&g_alarm_flags, ALARM_MEDIUM, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
        else if((adcData >= 524))
        { // High priority alarm
          alarm= 'H';
          OSFlagPost(&g_alarm_flags, ALARM_HIGH, OS_OPT_POST_FLAG_SET, &err);
          assert(OS_ERR_NONE == err);
        }
#endif
        
#ifdef DISPLAY_ADC
        sprintf(&msg[0], "ADC: %4u, %c", adcData, alarm);
        BSP_GraphLCD_String(5, (const char *)&msg);        
#endif
    }
}

/*!
*
* @brief ADC Interrupt Handler
*/
void
adc_isr (void)
{
    static uint16_t	 sample;    // NOTE: Not on the stack; so address is valid.
    OS_ERR	         err;


    // Read from the A/D converter and reduce the range from 12-bit to 10-bit.
    sample = adc.data[ADC_SOURCE_VR1] >> 2;
	
    // TODO: Send sample value to ADC task via a message queue
    OSQPost(&g_adc_q, (void *) sample, sizeof(uint16_t), OS_OPT_POST_FIFO, &err);
    assert(OS_ERR_NONE == err);
}
