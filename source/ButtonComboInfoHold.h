#pragma once

#include "ButtonComboInfo.h"

#include <buttoncombo/defines.h>

#include <span>
#include <string>

#include <cstdint>

class ButtonComboInfoHold final : public ButtonComboInfoIF {


public:
    ButtonComboInfoHold(std::string label,
                        ButtonComboModule_ControllerTypes controllerMask,
                        ButtonComboModule_Buttons combo,
                        uint32_t targetDuration,
                        ButtonComboModule_ComboCallback callback,
                        void *context,
                        bool observer);

    ~ButtonComboInfoHold() override;

private:
    void UpdateInput(ButtonComboModule_ControllerTypes controller, std::span<uint32_t> pressedButtons) override;

    ButtonComboModule_Error setHoldDuration(uint32_t holdDurationInFrames) override;

    [[nodiscard]] ButtonComboModule_ButtonComboInfoEx getComboInfoEx() const override;

    typedef struct {
        uint32_t durationInFrames;
        uint32_t prevButtonCombo;
        bool callbackTriggered;
    } HoldInformation;

    uint32_t mTargetDurationInFrames    = {};
    HoldInformation mHoldInformation[9] = {}; // one for each controller
};
