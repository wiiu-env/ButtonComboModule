#pragma once
#include <function_patcher/fpatching_defines.h>

#include <vpad/input.h>
#include <wut.h>

struct WUT_PACKED CCRCDCCallbackData {
    uint32_t attached;
    VPADChan chan;
    WUT_UNKNOWN_BYTES(6);
};

extern CCRCDCCallbackData gCCRCDCCallbackDataCurrent;
typedef void (*CCRCDCCallbackDataCallback)(CCRCDCCallbackData *, void *);

void InitDRCAttachCallbacks();

extern function_replacement_data_t drc_function_replacements[];
extern uint32_t drc_function_replacements_size;