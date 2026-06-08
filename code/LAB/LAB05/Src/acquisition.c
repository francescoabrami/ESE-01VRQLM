/*
 * acquisition.c
 *
 *  Created on: May 18, 2026
 *      Author: max
 */

#include "main.h"
#include "cmsis_os.h"
#include "lookup.h"

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

/** 
 * NOTES:
 * This is the old implementation of the acquisition task, which uses polling mode to read the ADC values. 
 * It is kept here for reference and testing purposes, but it is not used in the final implementation.
 * We are now using a LUT approach.
 */

/**

#define FDT_ADC (256.0f / 511875.0f)
#define R_DIV 10000.0f

// Declaration of output queue, defined in main.c

extern osMessageQueueId_t acquisitionQueueHandle;
extern osEventFlagsId_t MeasureEventFlagHandle;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

uint32_t read_adc_polling(ADC_HandleTypeDef hadc) {

    uint32_t adc_value = 0;
    HAL_ADC_Start(&hadc);

      if (HAL_ADC_PollForConversion(&hadc, 2) == HAL_OK){

          adc_value = HAL_ADC_GetValue(&hadc);

      }

    HAL_ADC_Stop(&hadc);

    return adc_value;
}

// Acquisition task entry point

void StartAcquisitionTask( void *argument ) {

  for( ; ; ) {

    osEventFlagsWait(MeasureEventFlagHandle, FLAG_MEASURE_READY, osFlagsWaitAny, osWaitForever);

    // Get the ADC values

    uint32_t measureADC1 = (uint32_t)read_adc_polling(hadc1);
    uint32_t measureADC2 = (uint32_t)read_adc_polling(hadc2);

    // We allign the value as (12-bit from ADC) XXXX_XXXX_XXXX_0000

    uint32_t Vntc32 = measureADC1;
    uint32_t Vcc32 = measureADC2 * 2;
    uint32_t Rntc32 = (Vntc32*10000)/(Vcc32-Vntc32); 
    const uint32_t R0 = 10000U; // 10kΩ at 25°C
    float Tntc_C = 25.0f + logf(Rntc32 / R0) / logf(0.96f);
    float Tntc_K = Tntc_C + 273.15f;

    //float Vntc = (float)measureADC1 * FDT_ADC;
    //float Vpow = (float)measureADC2 * FDT_ADC;
    //float Vcc = Vpow * 2.0f;
    //float Rntc = (Vntc / (Vcc - Vntc)) * R_DIV;
    //// It has a 10kΩ resistance at 25◦C. The value decreases by 4% for each increase
    //// of 1◦C of the component temperature.
    //// Model: R = R0 * (0.96)^(T_C - 25)
    //// => T_C = 25 - log(R / R0) / log(0.96)
    //// Convert to Kelvin: T_K = T_C + 273.15
    //const float R0 = 10000.0f; // 10kΩ at 25°C
    //float Tntc_C = 25.0f + logf(Rntc / R0) / logf(0.96f);
    //float Tntc_K = Tntc_C + 273.15f;

    // to keep a decimal digit, we multiply the temperature by 10 and round to the nearest integer
    uint16_t valueToSend = (uint16_t)roundf(Tntc_K * 10.0f);
    osMessageQueuePut(acquisitionQueueHandle, &valueToSend, 0, osWaitForever);
  }
}
 */