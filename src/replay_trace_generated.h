#ifndef EDGEAI_EV_REPLAY_TRACE_GENERATED_H
#define EDGEAI_EV_REPLAY_TRACE_GENERATED_H

#include <stdint.h>

typedef struct
{
    uint16_t current_mA;
    uint16_t power_mW;
    uint16_t voltage_mV;
    uint8_t soc_pct;
    uint8_t temp_c;
} replay_trace_point_t;

static const replay_trace_point_t kReplayTrace_Default[] = {
    {42, 510, 12100, 84, 29},
    {45, 545, 12110, 84, 29},
    {58, 705, 12120, 83, 30},
    {74, 900, 12130, 83, 30},
    {96, 1180, 12145, 82, 31},
    {118, 1450, 12160, 82, 31},
    {135, 1680, 12170, 81, 32},
    {148, 1840, 12180, 81, 32},
    {162, 2010, 12195, 80, 33},
    {173, 2150, 12200, 80, 33},
    {180, 2230, 12205, 80, 33},
    {166, 2060, 12190, 80, 33},
    {149, 1850, 12180, 79, 32},
    {131, 1630, 12165, 79, 32},
    {114, 1410, 12150, 79, 31},
    {95, 1170, 12135, 78, 31},
    {80, 980, 12125, 78, 30},
    {67, 820, 12118, 78, 30},
    {56, 690, 12110, 78, 30},
    {47, 575, 12105, 78, 29},
};

#define REPLAY_TRACE_DEFAULT_LEN ((uint32_t)(sizeof(kReplayTrace_Default) / sizeof(kReplayTrace_Default[0])))

#endif
