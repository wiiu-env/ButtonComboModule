#include "ButtonComboInfoHold.h"

#include <logger.h>

namespace {
    int32_t controllerTypeToChanIndex(const ButtonComboModule_ControllerTypes type) {
        switch (type) {
            case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0: {
                return 0;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1: {
                return 1;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0: {
                return 2;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1: {
                return 3;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2: {
                return 4;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3: {
                return 5;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4: {
                return 6;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5: {
                return 7;
            }
            case BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6: {
                return 8;
            }
            default:
                break;
        }

        return -1;
    }
} // namespace

ButtonComboInfoHold::ButtonComboInfoHold(std::string label,
                                         const ButtonComboModule_ControllerTypes controllerMask,
                                         const ButtonComboModule_Buttons combo,
                                         const uint32_t targetDurationInMs,
                                         const ButtonComboModule_ComboCallback callback,
                                         void *context,
                                         const bool observer) : ButtonComboInfoIF(std::move(label),
                                                                                  controllerMask,
                                                                                  combo,
                                                                                  callback,
                                                                                  context,
                                                                                  observer),
                                                                mTargetDurationInMs(targetDurationInMs) {
    DEBUG_FUNCTION_LINE_INFO("Created ButtonComboInfoDown: \"%s\", combo: %08X, targetDurationInMs: %d ms, controllerMask: %08X", mLabel.c_str(), mCombo, mTargetDurationInMs, mControllerMask);
}

ButtonComboInfoHold::~ButtonComboInfoHold() {
    DEBUG_FUNCTION_LINE_INFO("Deleted ButtonComboInfoDown: \"%s\", combo: %08X, targetDurationInMs: %d ms, controllerMask: %08X", mLabel.c_str(), mCombo, mTargetDurationInMs, mControllerMask);
}

void ButtonComboInfoHold::UpdateInput(const ButtonComboModule_ControllerTypes controller, const std::span<uint32_t> pressedButtons) {
    if ((mControllerMask & controller) == 0) {
        return;
    }
    const auto chanIndex = controllerTypeToChanIndex(controller);
    if (chanIndex < 0 || static_cast<uint32_t>(chanIndex) >= std::size(mHoldInformation)) {
        DEBUG_FUNCTION_LINE_WARN("ChanIndex is out of bounds %d", chanIndex);
        return;
    }

    auto &holdInformation        = mHoldInformation[chanIndex];
    const auto latestButtonPress = pressedButtons.back();

    DEBUG_FUNCTION_LINE_VERBOSE("[HOLD      ] Check button combo %08X on controller %08X (lastItem im pressedButtons (size %d) is %08X) for %s [%08X]", mCombo, controller, pressedButtons.size(), latestButtonPress, mLabel.c_str(), getHandle().handle);

    const bool prevButtonsIncludedCombo = (holdInformation.prevButtonCombo & mCombo) == mCombo; // Make sure the combo can't be triggered on releasing
    const bool buttonsPressedChanged    = holdInformation.prevButtonCombo != latestButtonPress; // Avoid "holding" the combo
    const bool buttonsPressedMatchCombo = latestButtonPress == mCombo;                          // detect the actual combo

    holdInformation.prevButtonCombo = latestButtonPress;

    if (!(buttonsPressedChanged && prevButtonsIncludedCombo) && (holdInformation.holdStartedAt > 0 || buttonsPressedChanged) && buttonsPressedMatchCombo) {
        if (holdInformation.holdStartedAt == 0) {
            holdInformation.holdStartedAt = OSGetTime();
        }
        const auto intervalInMs = static_cast<uint32_t>(OSTicksToMilliseconds(OSGetTime() - holdInformation.holdStartedAt));

        if (intervalInMs > mTargetDurationInMs && !holdInformation.callbackTriggered) {
            if (mCallback != nullptr) {
                DEBUG_FUNCTION_LINE("Calling callback [%08X](controller: %08X context: %08X) for \"%s\" [handle: %08X], hold %08X for %d ms", mCallback, controller, mContext, mLabel.c_str(), getHandle().handle, mCombo, intervalInMs);
                mCallback(controller, getHandle(), mContext);

            } else {
                DEBUG_FUNCTION_LINE_WARN("Callback was null for combo %08X", getHandle());
            }
            holdInformation.callbackTriggered = true;
        }
    } else {
        holdInformation.callbackTriggered = false;
        holdInformation.holdStartedAt     = 0;
    }
}

ButtonComboModule_Error ButtonComboInfoHold::setHoldDuration(const uint32_t holdDurationInMs) {
    DEBUG_FUNCTION_LINE("Setting holdDurationInMs to %d for %s [%08X]", holdDurationInMs, mLabel.c_str(), getHandle().handle);
    mTargetDurationInMs = holdDurationInMs;
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_ButtonComboInfoEx ButtonComboInfoHold::getComboInfoEx() const {
    return {.type               = mIsObserver ? BUTTON_COMBO_MODULE_TYPE_HOLD_OBSERVER : BUTTON_COMBO_MODULE_TYPE_HOLD,
            .basicCombo         = {.controllerMask = mControllerMask, .combo = mCombo},
            .optionalHoldForXMs = mTargetDurationInMs};
}
