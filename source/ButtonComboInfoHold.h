#pragma once

#include "ButtonComboInfo.h"

#include <buttoncombo/defines.h>

#include <span>
#include <string>

#include <coreinit/time.h>
#include <cstdint>

class ButtonComboInfoHold final : public ButtonComboInfoIF {


public:
    ButtonComboInfoHold(std::string label,
                        ButtonComboModule_ControllerTypes controllerMask,
                        ButtonComboModule_Buttons combo,
                        uint32_t targetDurationInMs,
                        ButtonComboModule_ComboCallback callback,
                        void *context,
                        bool observer);

    ~ButtonComboInfoHold() override;

private:
    void UpdateInput(ButtonComboModule_ControllerTypes controller, std::span<uint32_t> pressedButtons) override;

    ButtonComboModule_Error setHoldDuration(uint32_t holdDurationInMs) override;

    [[nodiscard]] ButtonComboModule_ButtonComboInfoEx getComboInfoEx() const override;

private:
    typedef struct {
        OSTime holdStartedAt;
        uint32_t prevButtonCombo;
        bool callbackTriggered;
    } HoldInformation;

    uint32_t mTargetDurationInMs        = {};
    HoldInformation mHoldInformation[9] = {}; // one for each controller
};
