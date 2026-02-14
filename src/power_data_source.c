#include "power_data_source.h"

#include "replay_trace_generated.h"

typedef struct
{
    power_data_source_mode_t mode;
    uint32_t replay_index;
    uint32_t tick_divider;
    power_sample_t current;
    power_sample_t live_override;
} power_data_state_t;

static power_data_state_t gPowerData;

static power_sample_t SampleFromReplay(uint32_t index)
{
    power_sample_t out;
    const replay_trace_point_t *src = &kReplayTrace_Default[index % REPLAY_TRACE_DEFAULT_LEN];

    out.current_mA = src->current_mA;
    out.power_mW = src->power_mW;
    out.voltage_mV = src->voltage_mV;
    out.soc_pct = src->soc_pct;
    out.temp_c = src->temp_c;
    return out;
}

void PowerData_Init(void)
{
    gPowerData.mode = POWER_DATA_SOURCE_REPLAY;
    gPowerData.replay_index = 0u;
    gPowerData.tick_divider = 0u;
    gPowerData.current = SampleFromReplay(0u);
    gPowerData.live_override = gPowerData.current;
}

void PowerData_SetMode(power_data_source_mode_t mode)
{
    gPowerData.mode = mode;
    if (mode == POWER_DATA_SOURCE_LIVE_OVERRIDE)
    {
        gPowerData.current = gPowerData.live_override;
    }
}

power_data_source_mode_t PowerData_GetMode(void)
{
    return gPowerData.mode;
}

void PowerData_SetLiveOverride(const power_sample_t *sample)
{
    if (sample == 0)
    {
        return;
    }

    gPowerData.live_override = *sample;
    if (gPowerData.mode == POWER_DATA_SOURCE_LIVE_OVERRIDE)
    {
        gPowerData.current = gPowerData.live_override;
    }
}

void PowerData_Tick(void)
{
    if (gPowerData.mode == POWER_DATA_SOURCE_LIVE_OVERRIDE)
    {
        gPowerData.current = gPowerData.live_override;
        return;
    }

    gPowerData.tick_divider++;
    if (gPowerData.tick_divider < 20000u)
    {
        return;
    }
    gPowerData.tick_divider = 0u;

    gPowerData.replay_index = (gPowerData.replay_index + 1u) % REPLAY_TRACE_DEFAULT_LEN;
    gPowerData.current = SampleFromReplay(gPowerData.replay_index);
}

const power_sample_t *PowerData_Get(void)
{
    return &gPowerData.current;
}

const char *PowerData_ModeName(void)
{
    return (gPowerData.mode == POWER_DATA_SOURCE_LIVE_OVERRIDE) ? "live_override" : "replay";
}
