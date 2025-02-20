#include "ButtonComboManager.h"
#include "globals.h"
#include "export.h"
#include "logger.h"
#include "TVOverlayManager.h"

#include <coreinit/cache.h>
#include <coreinit/debug.h>
#include <coreinit/time.h>

static ButtonComboModule_ComboHandle sTVButtonHandle;
static OSTime sTVPressed[2]; // when the button was last pressed, or zero if timeout expired
static bool sTVMenuBlocked[2];

static void TVComboCallback(ButtonComboModule_ControllerTypes triggeredBy,
                            ButtonComboModule_ComboHandle,
                            void *) {
    VPADChan chan;
    switch (triggeredBy) {
        case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0:
            chan = VPAD_CHAN_0;
            break;
        case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1:
            chan = VPAD_CHAN_1;
            break;
        default:
            return;
    }
    // OSReport("TV pressed\n");
    sTVPressed[chan] = OSGetSystemTime();
    OSMemoryBarrier();
}

void registerTVCombo() {
        ButtonComboModule_ComboOptions opt = {};
        opt.version = BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION;
        opt.metaOptions.label = "TV remote overlay combo";
        opt.callbackOptions.callback = TVComboCallback;
        opt.callbackOptions.context = {};
        opt.buttonComboOptions.type = BUTTON_COMBO_MODULE_COMBO_TYPE_PRESS_DOWN_OBSERVER;
        opt.buttonComboOptions.basicCombo.combo = BCMPAD_BUTTON_TV;
        opt.buttonComboOptions.basicCombo.controllerMask = BUTTON_COMBO_MODULE_CONTROLLER_VPAD;
        opt.buttonComboOptions.optionalHoldForXMs = 0;
        if (ButtonComboModule_AddButtonCombo(&opt, &sTVButtonHandle, nullptr) != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE("FAILED TO SET UP TV COMBO!");
        }
}

void unregisterTVCombo() {
    ButtonComboModule_RemoveButtonCombo(sTVButtonHandle);
}

void initTVStatus(VPADChan channel, bool block) {
    VPADSetTVMenuInvalid(channel, block);
    sTVMenuBlocked[channel] = block;

    sTVPressed[channel] = 0;
    OSMemoryBarrier();
}

void resetTVStatus(VPADChan channel) {
    sTVPressed[channel] = 0;
    OSMemoryBarrier();
}

void updateTVStatus(VPADChan channel) {
    if (sTVPressed[channel]) {
        uint64_t elapsed = OSGetSystemTime() - sTVPressed[channel];
        if (elapsed > OSMillisecondsToTicks(100) && sTVMenuBlocked[channel]) {
            // OSReport("TV menu unblocked\n");
            VPADSetTVMenuInvalid(channel, false);
            sTVMenuBlocked[channel] = false;
        }
        if (elapsed > OSMillisecondsToTicks(1000) && !VPADGetTVMenuStatus(channel)) {
            bool block = gButtonComboManager->hasActiveComboWithTVButton();
            // OSReport("TV timeout reached, setting TV Menu block to %s\n",
            //          block ? "blocked" : "unblocked");
            VPADSetTVMenuInvalid(channel, block);
            sTVMenuBlocked[channel] = block;
            sTVPressed[channel] = 0;
        }
    }
}
