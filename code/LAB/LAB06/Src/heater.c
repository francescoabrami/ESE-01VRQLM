#include "main.h"
#include "cmsis_os.h"
#include "io.h"
#include "stdbool.h"

#define TARGET_TEMPERATURE_10K 3232 // 323.2 K = 50 C
#define HYSTERESIS_10K 10 // +-1 K


extern osMessageQueueId_t controllQueueHandle;

void StartHeaterTask(void *argument){

    bool state = false;
    HAL_GPIO_WritePin(HeaterActive_GPIO_Port, HeaterActive_Pin, GPIO_PIN_RESET);

    for(;;)
    {
        uint16_t temperatureK = 0; // times 10
        osMessageQueueGet(controllQueueHandle, &temperatureK, NULL, osWaitForever);

        if(state && temperatureK > TARGET_TEMPERATURE_10K + HYSTERESIS_10K) {
            state = false;
            HAL_GPIO_WritePin(HeaterActive_GPIO_Port, HeaterActive_Pin, GPIO_PIN_RESET);
            led0( OFF );
        }
        else if(!state && temperatureK < TARGET_TEMPERATURE_10K - HYSTERESIS_10K) {
            state = true;
            HAL_GPIO_WritePin(HeaterActive_GPIO_Port, HeaterActive_Pin, GPIO_PIN_SET);
            led0( ON );
        }

    }


}