/*
 * application.c
 *
 *  Created on: Oct 20, 2020
 *      Author: max
 */

#include "main.h"
#include "cmsis_os2.h"
#include "io.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

extern COMP_HandleTypeDef hcomp1;

extern DAC_HandleTypeDef hdac1;

extern LCD_HandleTypeDef hlcd;

extern OPAMP_HandleTypeDef hopamp1;
extern OPAMP_HandleTypeDef hopamp2;

uint8_t counter;

void StartDefaultTask( void *argument ) {
	while( 1 ) {
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
}

