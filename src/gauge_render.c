#include "gauge_render.h"

#include <stdio.h>

#include "gauge_style.h"
#include "par_lcd_s035.h"
#include "text5x7.h"

static bool gLcdReady = false;
static bool gStaticReady = false;
static bool gDynamicReady = false;
static int32_t gPrevSpeedIdx = -1;
static int32_t gPrevPowerIdx = -1;
static int32_t gPrevSocIdx = -1;
static uint16_t gPrevCurrent = 0u;
static uint16_t gPrevPower = 0u;
static uint16_t gPrevVoltage = 0u;
static uint8_t gPrevSoc = 0u;

static int32_t ClampI32(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void DrawRing(int32_t cx, int32_t cy, int32_t r_outer, int32_t thickness, uint16_t ring, uint16_t inner)
{
    par_lcd_s035_draw_filled_circle(cx, cy, r_outer, ring);
    par_lcd_s035_draw_filled_circle(cx, cy, r_outer - thickness, inner);
}

static void DrawNeedleBand(int32_t cx, int32_t cy, int32_t len, int32_t idx, int32_t max_idx, uint16_t color)
{
    static const int8_t dx_lut[11] = {-52, -46, -34, -22, -8, 0, 9, 24, 37, 48, 54};
    static const int8_t dy_lut[11] = {16, -3, -21, -35, -46, -50, -46, -35, -21, -3, 16};
    int32_t x1;
    int32_t y1;
    int32_t steps;
    int32_t i;

    (void)len;
    idx = ClampI32(idx, 0, max_idx);
    x1 = cx + dx_lut[idx];
    y1 = cy + dy_lut[idx];
    steps = 22;

    for (i = 0; i <= steps; i++)
    {
        int32_t x = cx + ((x1 - cx) * i) / steps;
        int32_t y = cy + ((y1 - cy) * i) / steps;
        par_lcd_s035_fill_rect(x - 1, y - 1, x + 1, y + 1, color);
    }
    par_lcd_s035_draw_filled_circle(cx, cy, 5, color);
}

static void DrawGaugeTicks(int32_t cx, int32_t cy, uint16_t color)
{
    static const int8_t dx_lut[11] = {-58, -50, -38, -24, -9, 0, 10, 25, 39, 52, 60};
    static const int8_t dy_lut[11] = {18, -4, -24, -40, -52, -56, -52, -40, -24, -4, 18};
    int32_t i;
    for (i = 0; i < 11; i++)
    {
        int32_t x = cx + dx_lut[i];
        int32_t y = cy + dy_lut[i];
        par_lcd_s035_fill_rect(x - 1, y - 1, x + 1, y + 1, color);
    }
}

static void DrawStaticDashboard(const gauge_style_preset_t *style)
{
    par_lcd_s035_fill(style->palette.bg_black);
    par_lcd_s035_fill_rect(18, 16, 462, 304, style->palette.panel_black);
    par_lcd_s035_fill_rect(22, 20, 458, 46, style->palette.bezel_dark);
    edgeai_text5x7_draw_scaled(34, 26, 2, "EV CHARGE MONITOR", style->palette.text_primary);

    DrawRing(150, 170, 94, 12, style->palette.bezel_light, style->palette.panel_black);
    DrawRing(320, 170, 76, 10, style->palette.bezel_dark, style->palette.panel_black);
    DrawRing(418, 96, 44, 8, style->palette.bezel_light, style->palette.panel_black);

    DrawGaugeTicks(150, 170, style->palette.text_secondary);
    DrawGaugeTicks(320, 170, style->palette.text_secondary);

    edgeai_text5x7_draw_scaled(70, 278, 2, "CURRENT", style->palette.text_secondary);
    edgeai_text5x7_draw_scaled(252, 278, 2, "POWER", style->palette.text_secondary);
    edgeai_text5x7_draw_scaled(360, 20, 2, "SOC", style->palette.text_secondary);
    edgeai_text5x7_draw_scaled(330, 42, 2, "BATT MV", style->palette.text_secondary);
}

static void ClearValueBands(const gauge_style_preset_t *style)
{
    par_lcd_s035_fill_rect(60, 296, 230, 316, style->palette.panel_black);
    par_lcd_s035_fill_rect(240, 296, 410, 316, style->palette.panel_black);
    par_lcd_s035_fill_rect(350, 72, 462, 92, style->palette.panel_black);
    par_lcd_s035_fill_rect(350, 112, 448, 132, style->palette.panel_black);
}

bool GaugeRender_Init(void)
{
    const gauge_style_preset_t *style;

    gLcdReady = par_lcd_s035_init();
    if (gLcdReady)
    {
        style = GaugeStyle_GetCockpitPreset();
        DrawStaticDashboard(style);
        gStaticReady = true;
        gDynamicReady = false;
        gPrevSpeedIdx = -1;
        gPrevPowerIdx = -1;
        gPrevSocIdx = -1;
    }
    return gLcdReady;
}

void GaugeRender_DrawFrame(const power_sample_t *sample)
{
    const gauge_style_preset_t *style;
    int32_t speed_idx;
    int32_t power_idx;
    int32_t soc_idx;
    char line[32];

    if (!gLcdReady || sample == 0)
    {
        return;
    }

    style = GaugeStyle_GetCockpitPreset();

    if (!gStaticReady)
    {
        DrawStaticDashboard(style);
        gStaticReady = true;
        gDynamicReady = false;
        gPrevSpeedIdx = -1;
        gPrevPowerIdx = -1;
        gPrevSocIdx = -1;
    }

    speed_idx = ClampI32((int32_t)sample->current_mA / 20, 0, 10);
    power_idx = ClampI32((int32_t)sample->power_mW / 260, 0, 10);
    soc_idx = ClampI32((int32_t)sample->soc_pct / 10, 0, 10);

    if (gDynamicReady)
    {
        if (gPrevSpeedIdx != speed_idx)
        {
            DrawNeedleBand(150, 170, 52, gPrevSpeedIdx, 10, style->palette.panel_black);
            DrawNeedleBand(150, 170, 52, speed_idx, 10, style->palette.needle_amber);
            gPrevSpeedIdx = speed_idx;
        }
        if (gPrevPowerIdx != power_idx)
        {
            DrawNeedleBand(320, 170, 46, gPrevPowerIdx, 10, style->palette.panel_black);
            DrawNeedleBand(320, 170, 46, power_idx, 10, style->palette.accent_red);
            gPrevPowerIdx = power_idx;
        }
        if (gPrevSocIdx != soc_idx)
        {
            DrawNeedleBand(418, 96, 24, gPrevSocIdx, 10, style->palette.panel_black);
            DrawNeedleBand(418, 96, 24, soc_idx, 10, style->palette.accent_green);
            gPrevSocIdx = soc_idx;
        }
    }
    else
    {
        DrawNeedleBand(150, 170, 52, speed_idx, 10, style->palette.needle_amber);
        DrawNeedleBand(320, 170, 46, power_idx, 10, style->palette.accent_red);
        DrawNeedleBand(418, 96, 24, soc_idx, 10, style->palette.accent_green);
        gPrevSpeedIdx = speed_idx;
        gPrevPowerIdx = power_idx;
        gPrevSocIdx = soc_idx;
        gDynamicReady = true;
    }

    if (!gDynamicReady || gPrevCurrent != sample->current_mA || gPrevPower != sample->power_mW ||
        gPrevVoltage != sample->voltage_mV || gPrevSoc != sample->soc_pct)
    {
        ClearValueBands(style);
    }

    if (!gDynamicReady || gPrevCurrent != sample->current_mA)
    {
        snprintf(line, sizeof(line), "%4u MA", sample->current_mA);
        edgeai_text5x7_draw_scaled(66, 298, 2, line, style->palette.text_primary);
    }

    if (!gDynamicReady || gPrevPower != sample->power_mW)
    {
        snprintf(line, sizeof(line), "%4u MW", sample->power_mW);
        edgeai_text5x7_draw_scaled(246, 298, 2, line, style->palette.text_primary);
    }

    if (!gDynamicReady || gPrevSoc != sample->soc_pct)
    {
        snprintf(line, sizeof(line), "%3u%%", sample->soc_pct);
        edgeai_text5x7_draw_scaled(356, 74, 2, line, style->palette.text_primary);
    }

    if (!gDynamicReady || gPrevVoltage != sample->voltage_mV)
    {
        snprintf(line, sizeof(line), "%5u", sample->voltage_mV);
        edgeai_text5x7_draw_scaled(356, 114, 2, line, style->palette.accent_blue);
    }

    gPrevCurrent = sample->current_mA;
    gPrevPower = sample->power_mW;
    gPrevVoltage = sample->voltage_mV;
    gPrevSoc = sample->soc_pct;
}
