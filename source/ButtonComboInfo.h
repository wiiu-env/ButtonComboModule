#pragma once

#include <buttoncombo/defines.h>

#include <memory>
#include <span>
#include <string>

#include <cstdint>

class ButtonComboInfoIF {
public:
    ButtonComboInfoIF(std::string label,
                      ButtonComboModule_ControllerTypes controllerMask,
                      ButtonComboModule_Buttons combo,
                      ButtonComboModule_ComboCallback callback,
                      void *context,
                      bool observer);
    virtual ~ButtonComboInfoIF();
    virtual void UpdateInput(ButtonComboModule_ControllerTypes controller, std::span<uint32_t> pressedButtons) = 0;

    [[nodiscard]] bool isObserver() const;

    [[nodiscard]] ButtonComboModule_ComboHandle getHandle() const;
    [[nodiscard]] virtual const std::string &getLabel() const;

    [[nodiscard]] bool getMetaOptions(const ButtonComboModule_MetaOptionsOut &outMeta) const;
    void setMetaOptions(ButtonComboModule_MetaOptions options);

    [[nodiscard]] ButtonComboModule_CallbackOptions getCallbackOptions() const;
    void setCallbackOptions(ButtonComboModule_CallbackOptions options);

    [[nodiscard]] virtual uint32_t getCombo() const;
    void setCombo(ButtonComboModule_Buttons combo);

    [[nodiscard]] ButtonComboModule_ComboStatus getStatus() const;
    void setStatus(ButtonComboModule_ComboStatus status);

    [[nodiscard]] ButtonComboModule_ControllerTypes getControllerMask() const;
    void setControllerMask(ButtonComboModule_ControllerTypes combo);

    [[nodiscard]] bool conflictsWith(const ButtonComboModule_ButtonComboOptions &other) const;
    [[nodiscard]] bool conflictsWith(const ButtonComboInfoIF &other) const;

    virtual ButtonComboModule_Error setHoldDuration(uint32_t uint32) = 0;

    [[nodiscard]] virtual ButtonComboModule_ButtonComboInfoEx getComboInfoEx() const = 0;

protected:
    ButtonComboModule_ComboStatus mStatus = BUTTON_COMBO_MODULE_COMBO_STATUS_INVALID_STATUS;
    std::string mLabel;
    ButtonComboModule_ControllerTypes mControllerMask = {};
    ButtonComboModule_Buttons mCombo                  = {};
    ButtonComboModule_ComboCallback mCallback         = {};
    void *mContext                                    = {};
    bool mIsObserver                                  = {};
    std::unique_ptr<uint32_t> mHandle                 = std::make_unique<uint32_t>();
};
