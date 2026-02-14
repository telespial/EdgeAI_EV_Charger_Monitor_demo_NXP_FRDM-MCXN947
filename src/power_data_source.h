#ifndef EDGEAI_EV_POWER_DATA_SOURCE_H
#define EDGEAI_EV_POWER_DATA_SOURCE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t current_mA;
    uint16_t power_mW;
    uint16_t voltage_mV;
    uint8_t soc_pct;
    uint8_t temp_c;
} power_sample_t;

typedef enum
{
    POWER_DATA_SOURCE_REPLAY = 0,
    POWER_DATA_SOURCE_LIVE_OVERRIDE = 1,
} power_data_source_mode_t;

void PowerData_Init(void);
void PowerData_SetMode(power_data_source_mode_t mode);
power_data_source_mode_t PowerData_GetMode(void);
void PowerData_SetLiveOverride(const power_sample_t *sample);
void PowerData_Tick(void);
const power_sample_t *PowerData_Get(void);
const char *PowerData_ModeName(void);

#endif
