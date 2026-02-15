#ifndef EDGEAI_EV_REAL_TELEMETRY_H
#define EDGEAI_EV_REAL_TELEMETRY_H

#include <stdbool.h>

#include "power_data_source.h"

bool RealTelemetry_Init(void);
bool RealTelemetry_Sample(power_sample_t *sample);
bool RealTelemetry_IsReady(void);

#endif
