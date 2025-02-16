#include "ButtonComboInfo.h"
#include "ButtonComboManager.h"
#include "globals.h"

#include <function_patcher/fpatching_defines.h>
#include <logger.h>

#include <coreinit/cache.h>
#include <coreinit/time.h>
#include <padscore/wpad.h>
#include <vpad/input.h>

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffer, uint32_t buffer_size, VPADReadError *error) {
    VPADReadError real_error;
    const int32_t result = real_VPADRead(chan, buffer, buffer_size, &real_error);
    if (result > 0 && real_error == VPAD_READ_SUCCESS && gButtonComboManager) {
        gButtonComboManager->UpdateInputVPAD(chan, buffer, result > static_cast<int32_t>(buffer_size) ? buffer_size : result, error);
    }

    if (error) {
        *error = real_error;
    }

    if (gTVPressed[chan]) {
        uint64_t elapsed = OSGetSystemTime() - gTVPressed[chan];
        if (elapsed > OSMillisecondsToTicks(100) && gTVMenuBlocked[chan]) {
            OSReport("TV menu unblocked\n");
            VPADSetTVMenuInvalid(chan, false);
            gTVMenuBlocked[chan] = false;
        }
        if (elapsed > OSMillisecondsToTicks(1000) && !VPADGetTVMenuStatus(chan)) {
            bool block = gButtonComboManager->hasActiveComboWithTVButton();
            OSReport("TV timeout reached, setting TV Menu block to %s\n",
                     block ? "blocked" : "unblocked");
            VPADSetTVMenuInvalid(chan, block);
            gTVMenuBlocked[chan] = block;
            gTVPressed[chan] = 0;
            OSMemoryBarrier();
        }
    }

    return result;
}

DECL_FUNCTION(void, WPADRead, WPADChan chan, WPADStatus *data) {
    real_WPADRead(chan, data);

    if (gButtonComboManager) {
        gButtonComboManager->UpdateInputWPAD(chan, data);
    }
}
struct WUT_PACKED CCRCDCCallbackData {
    uint32_t attached;
    VPADChan chan;
    WUT_UNKNOWN_BYTES(6);
};

DECL_FUNCTION(void, __VPADBASEAttachCallback, CCRCDCCallbackData *data, void *context) {
    real___VPADBASEAttachCallback(data, context);

    if (data) {
        if (data->attached && gButtonComboManager) {
            const bool block = gButtonComboManager->hasActiveComboWithTVButton();
            VPADSetTVMenuInvalid(data->chan, block);
            gTVMenuBlocked[data->chan] = block;
        }
        gTVPressed[data->chan] = 0;
        OSMemoryBarrier();
    }
}

function_replacement_data_t function_replacements[] = {
        REPLACE_FUNCTION(VPADRead, LIBRARY_VPAD, VPADRead),
        REPLACE_FUNCTION(WPADRead, LIBRARY_PADSCORE, WPADRead),
        REPLACE_FUNCTION_VIA_ADDRESS(__VPADBASEAttachCallback, 0x31000000 + 0x0200146c - 0x00EE0100, 0x0200146c - 0x00EE0100),
};

uint32_t function_replacements_size = sizeof(function_replacements) / sizeof(function_replacement_data_t);
