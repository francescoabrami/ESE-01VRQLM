/*
 * application.c
 *
 *  Created on: Oct 20, 2020
 *      Author: max
 */

#include "main.h"
#include "cmsis_os2.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_tim_ex.h"
#include "io.h"

// Declare external references to timer instances

extern TIM_HandleTypeDef htim1;
extern osEventFlagsId_t MeasureEventFlagHandle;

// Callback used by the TIM1 on period interrupt
// Registered before starting timer1

void tim1Callback( TIM_HandleTypeDef *htim ) {
	osEventFlagsSet(MeasureEventFlagHandle, FLAG_MEASURE_READY);
}

// Declaration of queues, defined in main.c

extern osMessageQueueId_t acquisitionQueueHandle;
extern osMessageQueueId_t displayQueueHandle;
extern osMessageQueueId_t controllQueueHandle;

void StartDefaultTask( void *argument ) {
	while( 1 ) {
		if( getSwitch0() ) { // Blink LEDs
			while( getSwitch0() ) {
				led0( OFF );
				led1( ON );
				led2( OFF );
				led3( ON );
				osDelay( 500 );
				led0( ON );
				led1( OFF );
				led2( ON );
				led3( OFF );
				osDelay( 500 );
			}
		} else if( getSwitch1() ) { // This must be filled by students, to perform requested functions

			// Register timer interrupt callback
			HAL_TIM_RegisterCallback( &htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID, &tim1Callback );
			HAL_TIM_Base_Start_IT(&htim1);

			while( getSwitch1() ){
				uint16_t value;
				osMessageQueueGet(acquisitionQueueHandle, &value, NULL, osWaitForever);
				osMessageQueuePut(controllQueueHandle, &value, 0, osWaitForever);
				osMessageQueuePut(displayQueueHandle, &value, 0, osWaitForever);
			}
		}
	}
}

