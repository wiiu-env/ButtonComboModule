#pragma once
#include "ButtonComboInfo.h"

#include <padscore/wpad.h>
#include <vpad/input.h>

#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <cstdint>
#include <forward_list>


class ButtonComboManager {
public:
    ButtonComboManager();

    static std::optional<std::shared_ptr<ButtonComboInfoIF>> CreateComboInfo(const ButtonComboModule_ComboOptions &options, ButtonComboModule_Error &err);

    [[nodiscard]] ButtonComboInfoIF *GetComboInfoForHandle(ButtonComboModule_ComboHandle handle) const;

    void UpdateInputVPAD(VPADChan chan, const VPADStatus *buffer, uint32_t bufferSize, const VPADReadError *error);

    void UpdateInputWPAD(WPADChan chan, WPADStatus *data);

    bool hasActiveComboWithTVButton();

    ButtonComboModule_ComboStatus CheckComboStatus(const ButtonComboInfoIF &other);

    void AddCombo(std::shared_ptr<ButtonComboInfoIF> newComboInfo, ButtonComboModule_ComboHandle &outHandle, ButtonComboModule_ComboStatus &outStatus);

    ButtonComboModule_Error RemoveCombo(ButtonComboModule_ComboHandle handle);

    ButtonComboModule_Error GetButtonComboStatus(ButtonComboModule_ComboHandle handle, ButtonComboModule_ComboStatus &outComboStatus);

    ButtonComboModule_Error UpdateButtonComboMeta(ButtonComboModule_ComboHandle handle, const ButtonComboModule_MetaOptions &options);

    ButtonComboModule_Error UpdateButtonComboCallback(ButtonComboModule_ComboHandle handle, const ButtonComboModule_CallbackOptions &options);

    ButtonComboModule_Error UpdateControllerMask(ButtonComboModule_ComboHandle handle, ButtonComboModule_ControllerTypes controllerMask, ButtonComboModule_ComboStatus &outComboStatus);

    ButtonComboModule_Error UpdateButtonCombo(ButtonComboModule_ComboHandle handle, ButtonComboModule_Buttons combo, ButtonComboModule_ComboStatus &outComboStatus);

    ButtonComboModule_Error UpdateHoldDuration(ButtonComboModule_ComboHandle handle, uint32_t holdDurationInFrames);

    ButtonComboModule_Error GetButtonComboMeta(ButtonComboModule_ComboHandle handle, ButtonComboModule_MetaOptionsOut &outOptions);

    ButtonComboModule_Error GetButtonComboCallback(ButtonComboModule_ComboHandle handle, ButtonComboModule_CallbackOptions &outOptions);

    ButtonComboModule_Error GetButtonComboInfoEx(ButtonComboModule_ComboHandle handle, ButtonComboModule_ButtonComboInfoEx &outOptions);

    ButtonComboModule_ComboStatus CheckComboAvailable(const ButtonComboModule_ButtonComboOptions &other);

    ButtonComboModule_Error DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions &options, ButtonComboModule_Buttons &outButtonCombo);

private:
    std::forward_list<std::shared_ptr<ButtonComboInfoIF>> mCombos;
    std::vector<uint32_t> mVPADButtonBuffer;
    std::mutex mMutex;
};