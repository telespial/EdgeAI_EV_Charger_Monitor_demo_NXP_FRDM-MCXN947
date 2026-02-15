#include "power_data_source.h"

#include "replay_trace_generated.h"

#define POWER_SAMPLE_PERIOD_MS 50u
#define POWER_SIM_TIME_SCALE 60u

typedef struct
{
    power_data_source_mode_t mode;
    uint32_t replay_index;
    power_sample_t current;
    power_sample_t live_override;
    uint32_t elapsed_ms_real;
    uint32_t elapsed_ms_sim;
    uint32_t rng_state;
    int32_t filtered_current_mA;
    int32_t connector_temp_c10;
} power_data_state_t;

static power_data_state_t gPowerData;

static uint32_t NextRand(void)
{
    gPowerData.rng_state = (1664525u * gPowerData.rng_state) + 1013904223u;
    return gPowerData.rng_state;
}

static uint8_t ClampU8(int32_t v)
{
    if (v < 0)
    {
        return 0u;
    }
    if (v > 255)
    {
        return 255u;
    }
    return (uint8_t)v;
}

static power_sample_t SampleFromReplay(uint32_t index)
{
    power_sample_t out;
    const replay_trace_point_t *src = &kReplayTrace_Default[index % REPLAY_TRACE_DEFAULT_LEN];

    out.current_mA = src->current_mA;
    out.power_mW = src->power_mW;
    out.voltage_mV = src->voltage_mV;
    out.soc_pct = src->soc_pct;
    out.temp_c = src->temp_c;
    out.anomaly_score_pct = 0u;
    out.connector_wear_pct = 0u;
    out.elapsed_charge_s = 0u;
    out.elapsed_charge_sim_s = 0u;
    return out;
}

static void UpdateAiAndClock(const power_sample_t *prev)
{
    int32_t current_mA = (int32_t)gPowerData.current.current_mA;
    int32_t ambient_c10 = (int32_t)gPowerData.current.temp_c * 10;
    int32_t expected_rise_c10;
    int32_t expected_connector_c10;
    int32_t delta_i = 0;
    int32_t connector_excess_c10;
    int32_t anomaly;
    int32_t wear = (int32_t)gPowerData.current.connector_wear_pct;

    gPowerData.elapsed_ms_real += POWER_SAMPLE_PERIOD_MS;
    gPowerData.elapsed_ms_sim += POWER_SAMPLE_PERIOD_MS * POWER_SIM_TIME_SCALE;
    gPowerData.current.elapsed_charge_s = gPowerData.elapsed_ms_real / 1000u;
    gPowerData.current.elapsed_charge_sim_s = gPowerData.elapsed_ms_sim / 1000u;

    gPowerData.filtered_current_mA = ((gPowerData.filtered_current_mA * 7) + current_mA) / 8;
    expected_rise_c10 = gPowerData.filtered_current_mA / 2100;
    expected_connector_c10 = ambient_c10 + expected_rise_c10;

    if (prev != 0)
    {
        int32_t d = current_mA - (int32_t)prev->current_mA;
        delta_i = (d >= 0) ? d : -d;
    }

    if (current_mA > 30000)
    {
        wear += 1;
    }
    if ((delta_i > 3000) && (current_mA > 22000))
    {
        wear += 1;
    }
    if ((NextRand() & 0x3FFu) == 0u)
    {
        wear -= 1;
    }
    if (wear < 0)
    {
        wear = 0;
    }
    if (wear > 100)
    {
        wear = 100;
    }

    gPowerData.connector_temp_c10 = ((gPowerData.connector_temp_c10 * 9) + expected_connector_c10) / 10;
    gPowerData.connector_temp_c10 += wear / 3;
    gPowerData.connector_temp_c10 += delta_i / 2200;
    gPowerData.connector_temp_c10 += (int32_t)(NextRand() % 5u) - 2;

    if ((wear > 35) && ((NextRand() % 200u) == 0u))
    {
        gPowerData.connector_temp_c10 += 25;
    }

    connector_excess_c10 = gPowerData.connector_temp_c10 - expected_connector_c10;
    anomaly = connector_excess_c10 + (delta_i / 180) + (wear / 2);
    if (anomaly < 0)
    {
        anomaly = 0;
    }
    if (anomaly > 100)
    {
        anomaly = 100;
    }

    gPowerData.current.anomaly_score_pct = (uint16_t)anomaly;
    gPowerData.current.connector_wear_pct = ClampU8(wear);
}

void PowerData_Init(void)
{
    gPowerData.mode = POWER_DATA_SOURCE_REPLAY;
    gPowerData.replay_index = 0u;
    gPowerData.current = SampleFromReplay(0u);
    gPowerData.live_override = gPowerData.current;
    gPowerData.elapsed_ms_real = 0u;
    gPowerData.elapsed_ms_sim = 0u;
    gPowerData.rng_state = 0x43A17E5Bu;
    gPowerData.filtered_current_mA = (int32_t)gPowerData.current.current_mA;
    gPowerData.connector_temp_c10 = (int32_t)gPowerData.current.temp_c * 10;
    UpdateAiAndClock(0);
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
        UpdateAiAndClock(0);
    }
}

void PowerData_Tick(void)
{
    power_sample_t prev = gPowerData.current;

    if (gPowerData.mode == POWER_DATA_SOURCE_LIVE_OVERRIDE)
    {
        gPowerData.current = gPowerData.live_override;
        UpdateAiAndClock(&prev);
        return;
    }

    gPowerData.replay_index = (gPowerData.replay_index + 1u) % REPLAY_TRACE_DEFAULT_LEN;
    gPowerData.current = SampleFromReplay(gPowerData.replay_index);
    gPowerData.current.connector_wear_pct = prev.connector_wear_pct;
    UpdateAiAndClock(&prev);
}

const power_sample_t *PowerData_Get(void)
{
    return &gPowerData.current;
}

const char *PowerData_ModeName(void)
{
    return (gPowerData.mode == POWER_DATA_SOURCE_LIVE_OVERRIDE) ? "live_override" : "replay";
}
