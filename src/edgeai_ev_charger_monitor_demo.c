#include <stdbool.h>
#include <string.h>

#include "app.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "gauge_style.h"
#include "gauge_render.h"
#include "power_data_source.h"

#define POWER_SAMPLE_PERIOD_US 50000u

int main(void)
{
    uint32_t print_divider = 0u;
    bool lcd_ok;
    const power_sample_t *s;
    power_sample_t last_drawn;
    bool has_last = false;

    BOARD_InitHardware();

    PRINTF("EV Charger Monitor demo baseline booted\r\n");
    GaugeStyle_LogPreset();
    PowerData_Init();
    lcd_ok = GaugeRender_Init();
    PRINTF("Gauge render: %s\r\n", lcd_ok ? "ready" : "init_failed");
    PRINTF("Power data source: %s\r\n", PowerData_ModeName());

    s = PowerData_Get();
    if (lcd_ok)
    {
        GaugeRender_DrawFrame(s);
        last_drawn = *s;
        has_last = true;
    }

    for (;;)
    {
        PowerData_Tick();
        s = PowerData_Get();

        if (lcd_ok && (!has_last || memcmp(s, &last_drawn, sizeof(last_drawn)) != 0))
        {
            GaugeRender_DrawFrame(s);
            last_drawn = *s;
            has_last = true;
        }

        print_divider++;
        if (print_divider >= 20u)
        {
            print_divider = 0u;
            PRINTF("SAMPLE,mA=%u,mW=%u,mV=%u,SOC=%u,T=%u,AI=%u,WEAR=%u,sim_s=%u,mode=%s\r\n",
                   s->current_mA,
                   s->power_mW,
                   s->voltage_mV,
                   s->soc_pct,
                   s->temp_c,
                   s->anomaly_score_pct,
                   s->connector_wear_pct,
                   (unsigned int)s->elapsed_charge_sim_s,
                   PowerData_ModeName());
        }

        SDK_DelayAtLeastUs(POWER_SAMPLE_PERIOD_US, CLOCK_GetCoreSysClkFreq());
    }
}
