#include "real_telemetry.h"

#include "board.h"
#include "fsl_clock.h"
#include "fsl_lpadc.h"
#include "fsl_opamp.h"

#define TELEMETRY_ADC_BASE ADC0
#define TELEMETRY_OPAMP_BASE OPAMP0
#define TELEMETRY_ADC_TRIGGER_ID 0U
#define TELEMETRY_ADC_CMD_ID 1U
#define TELEMETRY_CURRENT_CHANNEL 1U /* ADC0_A1 is tied to OPAMP0_OUT on FRDM-MCXN947 schematic. */

#define TELEMETRY_ADC_FULL_SCALE 4095U
#define TELEMETRY_VREF_MV 3300U
#define TELEMETRY_SHUNT_MOHM 10U
#define TELEMETRY_OPAMP_GAIN 20U
#define TELEMETRY_ZERO_CAL_SAMPLES 96U
#define TELEMETRY_SOC_DEFAULT 80U
#define TELEMETRY_ADC_TIMEOUT 200000U

typedef struct
{
    bool ready;
    uint32_t zero_raw_acc;
    uint16_t zero_raw;
    uint32_t sample_count;
} real_telemetry_state_t;

static real_telemetry_state_t gTelemetry;

static bool Telemetry_ReadRaw(uint16_t *out_raw)
{
    lpadc_conv_result_t result;
    uint32_t timeout = TELEMETRY_ADC_TIMEOUT;

    if (out_raw == 0)
    {
        return false;
    }

    LPADC_DoSoftwareTrigger(TELEMETRY_ADC_BASE, 1UL << TELEMETRY_ADC_TRIGGER_ID);
    while (!LPADC_GetConvResult(TELEMETRY_ADC_BASE, &result, 0U))
    {
        if (timeout == 0u)
        {
            return false;
        }
        timeout--;
    }

    *out_raw = (uint16_t)(result.convValue >> 3U);
    return true;
}

bool RealTelemetry_Init(void)
{
    lpadc_config_t lpadc_config;
    lpadc_conv_command_config_t cmd_config;
    lpadc_conv_trigger_config_t trig_config;
    opamp_config_t opamp_config;
    uint32_t i;

    gTelemetry.ready = false;
    gTelemetry.zero_raw_acc = 0u;
    gTelemetry.zero_raw = 0u;
    gTelemetry.sample_count = 0u;

    CLOCK_AttachClk(kFRO_HF_to_ADC0);
    CLOCK_SetClkDiv(kCLOCK_DivAdc0Clk, 1u);
    RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);

    OPAMP_GetDefaultConfig(&opamp_config);
    opamp_config.posGain = kOPAMP_PosGainNonInvert1X;
    opamp_config.negGain = kOPAMP_NegGainInvert1X;
    opamp_config.enableOutputSwitch = true;
    opamp_config.enable = true;
    OPAMP_Init(TELEMETRY_OPAMP_BASE, &opamp_config);

    LPADC_GetDefaultConfig(&lpadc_config);
    lpadc_config.enableAnalogPreliminary = true;
    LPADC_Init(TELEMETRY_ADC_BASE, &lpadc_config);
    LPADC_DoAutoCalibration(TELEMETRY_ADC_BASE);

    LPADC_GetDefaultConvCommandConfig(&cmd_config);
    cmd_config.channelNumber = TELEMETRY_CURRENT_CHANNEL;
    cmd_config.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    cmd_config.hardwareAverageMode = kLPADC_HardwareAverageCount4;
    LPADC_SetConvCommandConfig(TELEMETRY_ADC_BASE, TELEMETRY_ADC_CMD_ID, &cmd_config);

    LPADC_GetDefaultConvTriggerConfig(&trig_config);
    trig_config.targetCommandId = TELEMETRY_ADC_CMD_ID;
    trig_config.enableHardwareTrigger = false;
    LPADC_SetConvTriggerConfig(TELEMETRY_ADC_BASE, TELEMETRY_ADC_TRIGGER_ID, &trig_config);

    for (i = 0u; i < TELEMETRY_ZERO_CAL_SAMPLES; i++)
    {
        uint16_t raw;
        if (!Telemetry_ReadRaw(&raw))
        {
            return false;
        }
        gTelemetry.zero_raw_acc += raw;
    }
    gTelemetry.zero_raw = (uint16_t)(gTelemetry.zero_raw_acc / TELEMETRY_ZERO_CAL_SAMPLES);
    gTelemetry.ready = true;
    return true;
}

bool RealTelemetry_Sample(power_sample_t *sample)
{
    int32_t delta_raw;
    uint32_t delta_mV;
    uint32_t current_mA;
    uint32_t power_mW;
    uint32_t temp_c;
    uint16_t raw;

    if ((!gTelemetry.ready) || (sample == 0))
    {
        return false;
    }

    if (!Telemetry_ReadRaw(&raw))
    {
        return false;
    }
    gTelemetry.sample_count++;

    delta_raw = (int32_t)raw - (int32_t)gTelemetry.zero_raw;
    if (delta_raw < 0)
    {
        delta_raw = 0;
    }

    delta_mV = ((uint32_t)delta_raw * TELEMETRY_VREF_MV) / TELEMETRY_ADC_FULL_SCALE;
    current_mA = (delta_mV * 1000u) / (TELEMETRY_SHUNT_MOHM * TELEMETRY_OPAMP_GAIN);
    if (current_mA > 65535u)
    {
        current_mA = 65535u;
    }

    sample->voltage_mV = TELEMETRY_VREF_MV;
    sample->current_mA = (uint16_t)current_mA;

    power_mW = (current_mA * sample->voltage_mV) / 1000u;
    if (power_mW > 65535u)
    {
        power_mW = 65535u;
    }
    sample->power_mW = (uint16_t)power_mW;

    temp_c = 30u + (current_mA / 180u);
    if (temp_c > 95u)
    {
        temp_c = 95u;
    }
    sample->temp_c = (uint8_t)temp_c;
    sample->soc_pct = TELEMETRY_SOC_DEFAULT;
    return true;
}

bool RealTelemetry_IsReady(void)
{
    return gTelemetry.ready;
}
