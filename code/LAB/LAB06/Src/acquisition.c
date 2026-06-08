/*
 * acquisition.c
 *
 *  Created on: May 18, 2026
 *      Author: max
 */

#include "main.h"
#include "cmsis_os.h"
#include "lookup.h"
#include <math.h>

#define FDT_ADC (256.0f / 511875.0f)
#define R_DIV 10000.0f

// Declaration of output queue, defined in main.c

extern osMessageQueueId_t acquisitionQueueHandle;
extern osEventFlagsId_t MeasureEventFlagHandle;
extern osEventFlagsId_t AdcDrdyEventFlagHandle;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

// Function to read the value from the ADC in polling mode, used for testing
uint16_t measureADC1;
uint16_t measureADC2;

// Callback used by the ADC on conversion complete interrupt
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if (hadc->Instance == ADC1) {
    /* Get the converted value of regular channel */
    measureADC1 = HAL_ADC_GetValue(hadc);
    osEventFlagsSet(AdcDrdyEventFlagHandle, FLAG_ADC1_DRDY);
  }
  else if (hadc->Instance == ADC2) {
    /* Get the converted value of regular channel */
    measureADC2 = HAL_ADC_GetValue(hadc);
    osEventFlagsSet(AdcDrdyEventFlagHandle, FLAG_ADC2_DRDY);
  }
}

uint32_t read_adc_polling(ADC_HandleTypeDef hadc) {
    uint32_t adc_value = 0;
    HAL_ADC_Start(&hadc);
      if (HAL_ADC_PollForConversion(&hadc, 2) == HAL_OK){
          adc_value = HAL_ADC_GetValue(&hadc);
      } // else adc_value=0;
    HAL_ADC_Stop(&hadc);
    return adc_value;
}

// Acquisition task entry point

void StartAcquisitionTask( void *argument ) {
  /* Infinite loop */

  for( ; ; ) {
    osEventFlagsWait(MeasureEventFlagHandle, FLAG_MEASURE_READY, osFlagsWaitAny, osWaitForever);

    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);

    osEventFlagsWait(AdcDrdyEventFlagHandle, FLAG_ADC1_DRDY | FLAG_ADC2_DRDY, osFlagsWaitAll, osWaitForever);

    uint32_t Vntc32 = measureADC1;
    uint32_t Vcc32 = measureADC2 * 2;
    uint32_t Rntc32 = (Vntc32*10000)/(Vcc32-Vntc32); 

    uint16_t Tntc_K = GetTemperature(Rntc32);

    osMessageQueuePut(acquisitionQueueHandle, &Tntc_K, 0, osWaitForever);
  }
}