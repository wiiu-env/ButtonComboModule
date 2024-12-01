#include "ButtonComboInfoDown.h"

#include "logger.h"

ButtonComboInfoDown::ButtonComboInfoDown(
        std::string label,
        const ButtonComboModule_ControllerTypes controllerMask,
        const ButtonComboModule_Buttons combo,
        const ButtonComboModule_ComboCallback callback,
        void *context,
        const bool observer) : ButtonComboInfoIF(std::move(label), controllerMask, combo, callback, context, observer) {
}

void ButtonComboInfoDown::UpdateInput(
        const ButtonComboModule_ControllerTypes controller,
        const std::span<uint32_t> pressedButtons) {
    if ((mControllerMask & controller) == 0) {
        return;
    }

    for (const auto &pressedButton : pressedButtons) {
        const bool prevButtonsIncludedCombo = (mPrevButtonPress & mCombo) == mCombo; // Make sure the combo can't be triggered on releasing
        const bool buttonsPressedChanged    = mPrevButtonPress != pressedButton;     // Avoid "holding" the combo
        const bool buttonsPressedMatchCombo = pressedButton == mCombo;               // detect the actual combo

        if (buttonsPressedChanged && buttonsPressedMatchCombo && !prevButtonsIncludedCombo) {
            if (mCallback != nullptr) {
                mCallback(getHandle(), mContext);
            } else {
                DEBUG_FUNCTION_LINE_WARN("Callback was null for combo %08X", getHandle());
            }
        }
        mPrevButtonPress = pressedButton;
    }
}

ButtonComboModule_Error ButtonComboInfoDown::setHoldDuration(uint32_t) {
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}
ButtonComboModule_ButtonComboInfoEx ButtonComboInfoDown::getComboInfoEx() const {
    return {.type                   = mIsObserver ? BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN_OBSERVER : BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN,
            .basicCombo             = {.controllerMask = mControllerMask, .combo = mCombo},
            .optionalHoldForXFrames = 0};
}
