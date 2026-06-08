/*
 * acquisition.c
 *
 *  Created on: May 18, 2026
 *      Author: max
 */

#include "main.h"
#include "cmsis_os.h"
#include "io.h"

// Declaration of input queue, defined in main.c

extern osMessageQueueId_t displayQueueHandle;

void StartDisplayTask( void *argument ) {
  /* Infinite loop */
  for( ; ; ) {
    uint16_t value;
    osMessageQueueGet(displayQueueHandle, &value, NULL, osWaitForever);
    uint8_t digits[4];
    // The value is the temperature in Kelvin multiplied by 10
    // the last digit is the decimal part, the first three digits are the integer part

    digits[0] = (value / 1000) % 10 + '0';
    digits[1] = (value / 100) % 10 + '0';
    digits[2] = (value / 10) % 10 + '0';
    digits[3] = value % 10 + '0';

    lcdWriteDigit(digits[0], 0);
    lcdWriteDigit(digits[1], 1);
    lcdWriteDigit(digits[2], 2);
    lcdWriteDigit(digits[3], 3);

    lcdUpdateDisplay();
  }
}

