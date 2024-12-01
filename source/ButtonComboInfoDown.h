#pragma once
#include "ButtonComboInfo.h"

#include <buttoncombo/defines.h>

#include <span>
#include <string>

#include <cstdint>

class ButtonComboInfoDown final : public ButtonComboInfoIF {
public:
    ButtonComboInfoDown(std::string label,
                        ButtonComboModule_ControllerTypes controllerMask,
                        ButtonComboModule_Buttons combo,
                        ButtonComboModule_ComboCallback callback,
                        void *context,
                        bool observer);

private:
    void UpdateInput(ButtonComboModule_ControllerTypes controller, std::span<uint32_t> pressedButtons) override;

    ButtonComboModule_Error setHoldDuration(uint32_t uint32) override;

    [[nodiscard]] ButtonComboModule_ButtonComboInfoEx getComboInfoEx() const override;

    uint32_t mPrevButtonPress = {};
};