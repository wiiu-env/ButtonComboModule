#include "TVOverlayManager.h"
#include "ButtonComboManager.h"
#include "export.h"
#include "globals.h"
#include "logger.h"

#include <coreinit/cache.h>
#include <coreinit/debug.h>
#include <coreinit/time.h>

namespace TVOverlayManager {

    namespace {

        ButtonComboModule_ComboHandle sTVButtonHandle;
        // Track when the TV button was last pressed to implement timeout, or zero if
        // timeout expired.
        OSTime sTVPressed[2];
        bool sTVMenuBlocked[2];

        void TVComboCallback(ButtonComboModule_ControllerTypes triggeredBy,
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
            DEBUG_FUNCTION_LINE_INFO("TV pressed");
            sTVPressed[chan] = OSGetSystemTime();
            OSMemoryBarrier();
        }

        void SetBlocked(VPADChan channel, bool blocked) {
            VPADSetTVMenuInvalid(channel, blocked);
            sTVMenuBlocked[channel] = blocked;
        }

    } // namespace

    void RegisterCombo() {
        ButtonComboModule_ComboOptions opt               = {};
        opt.version                                      = BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION;
        opt.metaOptions.label                            = "TV remote overlay combo";
        opt.callbackOptions.callback                     = TVComboCallback;
        opt.callbackOptions.context                      = {};
        opt.buttonComboOptions.type                      = BUTTON_COMBO_MODULE_COMBO_TYPE_PRESS_DOWN_OBSERVER;
        opt.buttonComboOptions.basicCombo.combo          = BCMPAD_BUTTON_TV;
        opt.buttonComboOptions.basicCombo.controllerMask = BUTTON_COMBO_MODULE_CONTROLLER_VPAD;
        opt.buttonComboOptions.optionalHoldForXMs        = 0;
        if (ButtonComboModule_AddButtonCombo(&opt, &sTVButtonHandle, nullptr) != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("FAILED TO SET UP TV COMBO!");
        }
    }

    void UnregisterCombo() {
        ButtonComboModule_RemoveButtonCombo(sTVButtonHandle);
    }

    void InitVPAD(VPADChan channel) {
        bool blocked = false;
        if (auto mgr = gButtonComboManager) {
            blocked = mgr->hasActiveComboWithTVButton();
        }
        SetBlocked(channel, blocked);
        ResetVPAD(channel);
    }

    void ResetVPAD(VPADChan channel) {
        sTVPressed[channel] = 0;
        OSMemoryBarrier();
    }

    void UpdateVPAD(VPADChan channel) {
        if (sTVPressed[channel]) {
            OSTime elapsed = OSGetSystemTime() - sTVPressed[channel];
            // TODO: this should be configurable by the public API.
            const OSTime double_press_time = OSMillisecondsToTicks(100);
            if (elapsed > double_press_time && sTVMenuBlocked[channel]) {
                DEBUG_FUNCTION_LINE_INFO("Unblocking TV menu");
                SetBlocked(channel, false);
            }
            // TODO: this should be configurable by the public API.
            const OSTime tv_enable_timeout = OSMillisecondsToTicks(1000);
            if (elapsed > tv_enable_timeout && !VPADGetTVMenuStatus(channel)) {
                bool blocked = false;
                if (auto mgr = gButtonComboManager) {
                    blocked = mgr->hasActiveComboWithTVButton();
                }
                DEBUG_FUNCTION_LINE_INFO("TV timeout reached, setting TV Menu block to %s",
                                         blocked ? "blocked" : "unblocked");
                SetBlocked(channel, blocked);
                ResetVPAD(channel);
            }
        }
    }

    void UpdateBlocking() {
        bool blocked = false;
        if (auto mgr = gButtonComboManager) {
            blocked = mgr->hasActiveComboWithTVButton();
        }
        for (VPADChan channel : {VPAD_CHAN_0, VPAD_CHAN_1}) {
            SetBlocked(channel, blocked);
            ResetVPAD(channel);
        }
    }

} // namespace TVOverlayManager
