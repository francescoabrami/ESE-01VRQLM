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

// Callback used by the TIM1 on period interrupt
// Registered before starting timer1

void tim1Callback( TIM_HandleTypeDef *htim ) {
	HAL_GPIO_WritePin( IO10_GPIO_GPIO_Port, IO10_GPIO_Pin, GPIO_PIN_SET );
	HAL_GPIO_WritePin( IO10_GPIO_GPIO_Port, IO10_GPIO_Pin, GPIO_PIN_RESET );
}

// Callback used by the TIM4 on period interrupt
// Directly linked in stm32l4xx_it.c file

void tim4PeriodCallback() {
	LL_GPIO_SetOutputPin( IO29_GPIO_GPIO_Port, IO29_GPIO_Pin );
	LL_GPIO_ResetOutputPin( IO29_GPIO_GPIO_Port, IO29_GPIO_Pin );
}

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
		} else if( getSwitch1() ) { // Toggle GPIO using HAL toggle function
			while( getSwitch1() ) {
				HAL_GPIO_TogglePin( IO10_GPIO_GPIO_Port, IO10_GPIO_Pin );
			}
		} else if( getSwitch2() ) { // Toggle GPIO using LL toggle function
			while( getSwitch2() ) {
				LL_GPIO_TogglePin( IO29_GPIO_GPIO_Port, IO29_GPIO_Pin );
			}
		} else if( getSwitch3() ) { // Toggle GPIO using LL set/reset output function
			while( getSwitch3() ) {
				LL_GPIO_SetOutputPin( IO29_GPIO_GPIO_Port, IO29_GPIO_Pin );
				LL_GPIO_ResetOutputPin( IO29_GPIO_GPIO_Port, IO29_GPIO_Pin );
			}
		} else if( getSwitch4() ) { // Toggle GPIO using TIM1 and HAL API
			// Stop TIM1 on debug breakpoint
			__HAL_DBGMCU_FREEZE_TIM1();
			const int period = 1000;
			// Set period, clear timer counter
			__HAL_TIM_SET_AUTORELOAD( &htim1, period );
			__HAL_TIM_SET_COUNTER( &htim1, 0 );
			__HAL_TIM_SET_COMPARE( &htim1, TIM_CHANNEL_1, period / 2 );
			// Register timer interrupt callback
			HAL_TIM_RegisterCallback( &htim1, HAL_TIM_PWM_PULSE_FINISHED_CB_ID, &tim1Callback );
			// Start timer, generating output on IO9, interrupts, and related callback invocation
			HAL_TIM_PWM_Start_IT( &htim1, TIM_CHANNEL_1 );
			// Wait until switch is deasserted
			while( getSwitch4() );
			// Stop timer
			HAL_TIM_PWM_Stop_IT( &htim1, TIM_CHANNEL_1 );
		} else if( getSwitch5() ) { // Toggle GPIO using TIM4 and LL API
			// Stop TIM4 on debug breakpoint
			__HAL_DBGMCU_FREEZE_TIM4();
			const int period = 10000;
			// Set period, clear timer counter
			LL_TIM_SetAutoReload( TIM4, period );
			LL_TIM_SetCounter( TIM4, 0 );
			LL_TIM_OC_SetCompareCH1( TIM4, period / 2 );
			// Start timer, generating output on IO9, interrupts, and related callback invocation
			LL_TIM_EnableCounter( TIM4 );
			LL_TIM_EnableAllOutputs( TIM4 );
			// Wait until switch is deasserted
			while( getSwitch5() );
			// Stop timer
			LL_TIM_DisableCounter( TIM4 );
		}
	}
}

