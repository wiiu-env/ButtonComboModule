#include "ButtonComboInfoDown.h"

#include "logger.h"

ButtonComboInfoDown::ButtonComboInfoDown(
        std::string label,
        const ButtonComboModule_ControllerTypes controllerMask,
        const ButtonComboModule_Buttons combo,
        const ButtonComboModule_ComboCallback callback,
        void *context,
        const bool observer) : ButtonComboInfoIF(std::move(label), controllerMask, combo, callback, context, observer) {
    DEBUG_FUNCTION_LINE_INFO("Created ButtonComboInfoDown: \"%s\", combo: %08X, controllerMask: %08X. Observer %d", mLabel.c_str(), mCombo, mControllerMask, observer);
}

ButtonComboInfoDown::~ButtonComboInfoDown() {
    DEBUG_FUNCTION_LINE_INFO("Deleted ButtonComboInfoDown: \"%s\", combo: %08X, controllerMask: %08X.  Observer %d", mLabel.c_str(), mCombo, mControllerMask, mIsObserver);
}

void ButtonComboInfoDown::UpdateInput(
        const ButtonComboModule_ControllerTypes controller,
        const std::span<uint32_t> pressedButtons) {
    if ((mControllerMask & controller) == 0) {
        return;
    }
    const auto chanIndex = ControllerTypeToChanIndex(controller);
    if (chanIndex < 0 || static_cast<uint32_t>(chanIndex) >= std::size(mHoldInformation)) {
        DEBUG_FUNCTION_LINE_WARN("ChanIndex is out of bounds %d", chanIndex);
        return;
    }

    auto &[prevButtonCombo] = mHoldInformation[chanIndex];

    DEBUG_FUNCTION_LINE_VERBOSE("[PRESS DOWN] Check button combo %08X on controller %08X (lastItem im pressedButtons (size %d) is %08X) for %s [%08X]", mCombo, controller, pressedButtons.size(), pressedButtons.back(), mLabel.c_str(), getHandle().handle);

    for (const auto &pressedButton : pressedButtons) {
        const bool prevButtonsIncludedCombo = (prevButtonCombo & mCombo) == mCombo; // Make sure the combo can't be triggered on releasing
        const bool buttonsPressedChanged    = prevButtonCombo != pressedButton;     // Avoid "holding" the combo
        const bool buttonsPressedMatchCombo = pressedButton == mCombo;              // detect the actual combo

        if (buttonsPressedChanged && buttonsPressedMatchCombo && !prevButtonsIncludedCombo) {
            if (mCallback != nullptr) {
                DEBUG_FUNCTION_LINE("Calling callback [%08X](controller: %08X, context: %08X) for \"%s\" [handle: %08X], pressed down %08X", mCallback, controller, mContext, mLabel.c_str(), getHandle().handle, mCombo);
                mCallback(controller, getHandle(), mContext);
            } else {
                DEBUG_FUNCTION_LINE_WARN("Callback was null for combo %08X", getHandle());
            }
        }
        prevButtonCombo = pressedButton;
    }
}

ButtonComboModule_Error ButtonComboInfoDown::setHoldDuration(uint32_t) {
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_ButtonComboInfoEx ButtonComboInfoDown::getComboInfoEx() const {
    return {.type               = mIsObserver ? BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN_OBSERVER : BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN,
            .basicCombo         = {.controllerMask = mControllerMask, .combo = mCombo},
            .optionalHoldForXMs = 0};
}
