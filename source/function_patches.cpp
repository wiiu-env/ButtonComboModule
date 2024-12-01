#include "ButtonComboInfo.h"
#include "ButtonComboManager.h"
#include "DRCAttachCallback.h"
#include "globals.h"

#include <function_patcher/fpatching_defines.h>

#include <coreinit/messagequeue.h>
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
    return result;
}

DECL_FUNCTION(void, WPADRead, WPADChan chan, WPADStatus *data) {
    real_WPADRead(chan, data);

    if (gButtonComboManager) {
        gButtonComboManager->UpdateInputWPAD(chan, data);
    }
}


static uint32_t lastData0 = 0;
DECL_FUNCTION(uint32_t, OSReceiveMessage, OSMessageQueue *queue, OSMessage *message, uint32_t flags) {
    const uint32_t res = real_OSReceiveMessage(queue, message, flags);
    if (queue == OSGetSystemMessageQueue()) {
        if (message != nullptr && res) {
            if (lastData0 != message->args[0]) {
                if (message->args[0] == 0xFACEF000) {
                    InitDRCAttachCallbacks();
                }
            }
            lastData0 = message->args[0];
        }
    }
    return res;
}

function_replacement_data_t function_replacements[] = {
        REPLACE_FUNCTION(VPADRead, LIBRARY_VPAD, VPADRead),
        REPLACE_FUNCTION(WPADRead, LIBRARY_PADSCORE, WPADRead),
        REPLACE_FUNCTION(OSReceiveMessage, LIBRARY_COREINIT, OSReceiveMessage),
};

uint32_t function_replacements_size = sizeof(function_replacements) / sizeof(function_replacement_data_t);
