#include "ButtonComboManager.h"
#include "ButtonComboInfoDown.h"
#include "ButtonComboInfoHold.h"
#include "logger.h"

#include <buttoncombo/defines.h>

#include <coreinit/thread.h>
#include <padscore/kpad.h>

#include <algorithm>
#include <vector>

namespace {
    template<typename Container, typename Predicate>
    std::enable_if_t<std::is_same_v<Container, std::forward_list<typename Container::value_type>>, bool>
    remove_first_if(Container &container, Predicate pred) {
        auto it = container.before_begin();

        for (auto prev = it, current = ++it; current != container.end(); ++prev, ++current) {
            if (pred(*current)) {
                container.erase_after(prev);
                return true;
            }
        }

        return false;
    }

    uint32_t remapVPADButtons(const uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & VPAD_BUTTON_A) {
            conv_buttons |= BCMPAD_BUTTON_A;
        }
        if (buttons & VPAD_BUTTON_B) {
            conv_buttons |= BCMPAD_BUTTON_B;
        }
        if (buttons & VPAD_BUTTON_X) {
            conv_buttons |= BCMPAD_BUTTON_X;
        }
        if (buttons & VPAD_BUTTON_Y) {
            conv_buttons |= BCMPAD_BUTTON_Y;
        }

        if (buttons & VPAD_BUTTON_LEFT) {
            conv_buttons |= BCMPAD_BUTTON_LEFT;
        }
        if (buttons & VPAD_BUTTON_RIGHT) {
            conv_buttons |= BCMPAD_BUTTON_RIGHT;
        }
        if (buttons & VPAD_BUTTON_UP) {
            conv_buttons |= BCMPAD_BUTTON_UP;
        }
        if (buttons & VPAD_BUTTON_DOWN) {
            conv_buttons |= BCMPAD_BUTTON_DOWN;
        }

        if (buttons & VPAD_BUTTON_ZL) {
            conv_buttons |= BCMPAD_BUTTON_ZL;
        }
        if (buttons & VPAD_BUTTON_ZR) {
            conv_buttons |= BCMPAD_BUTTON_ZR;
        }

        if (buttons & VPAD_BUTTON_L) {
            conv_buttons |= BCMPAD_BUTTON_L;
        }
        if (buttons & VPAD_BUTTON_R) {
            conv_buttons |= BCMPAD_BUTTON_R;
        }

        if (buttons & VPAD_BUTTON_PLUS) {
            conv_buttons |= BCMPAD_BUTTON_PLUS;
        }
        if (buttons & VPAD_BUTTON_MINUS) {
            conv_buttons |= BCMPAD_BUTTON_MINUS;
        }

        if (buttons & VPAD_BUTTON_STICK_R) {
            conv_buttons |= BCMPAD_BUTTON_STICK_R;
        }
        if (buttons & VPAD_BUTTON_STICK_L) {
            conv_buttons |= BCMPAD_BUTTON_STICK_L;
        }

        if (buttons & VPAD_BUTTON_TV) {
            conv_buttons |= BCMPAD_BUTTON_TV;
        }

        return conv_buttons;
    }

    uint32_t remapWiiMoteButtons(const uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & WPAD_BUTTON_LEFT) {
            conv_buttons |= BCMPAD_BUTTON_LEFT;
        }

        if (buttons & WPAD_BUTTON_RIGHT) {
            conv_buttons |= BCMPAD_BUTTON_RIGHT;
        }

        if (buttons & WPAD_BUTTON_DOWN) {
            conv_buttons |= BCMPAD_BUTTON_DOWN;
        }

        if (buttons & WPAD_BUTTON_UP) {
            conv_buttons |= BCMPAD_BUTTON_UP;
        }

        if (buttons & WPAD_BUTTON_PLUS) {
            conv_buttons |= BCMPAD_BUTTON_PLUS;
        }

        if (buttons & WPAD_BUTTON_B) {
            conv_buttons |= BCMPAD_BUTTON_B;
        }

        if (buttons & WPAD_BUTTON_A) {
            conv_buttons |= BCMPAD_BUTTON_A;
        }

        if (buttons & WPAD_BUTTON_MINUS) {
            conv_buttons |= BCMPAD_BUTTON_MINUS;
        }

        return conv_buttons;
    }

    uint32_t remapClassicButtons(const uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & WPAD_CLASSIC_BUTTON_LEFT) {
            conv_buttons |= BCMPAD_BUTTON_LEFT;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_RIGHT) {
            conv_buttons |= BCMPAD_BUTTON_RIGHT;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_DOWN) {
            conv_buttons |= BCMPAD_BUTTON_DOWN;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_UP) {
            conv_buttons |= BCMPAD_BUTTON_UP;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_PLUS) {
            conv_buttons |= BCMPAD_BUTTON_PLUS;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_X) {
            conv_buttons |= BCMPAD_BUTTON_X;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_Y) {
            conv_buttons |= BCMPAD_BUTTON_Y;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_B) {
            conv_buttons |= BCMPAD_BUTTON_B;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_A) {
            conv_buttons |= BCMPAD_BUTTON_A;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_MINUS) {
            conv_buttons |= BCMPAD_BUTTON_MINUS;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_ZR) {
            conv_buttons |= BCMPAD_BUTTON_ZR;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_ZL) {
            conv_buttons |= BCMPAD_BUTTON_ZL;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_R) {
            conv_buttons |= BCMPAD_BUTTON_R;
        }
        if (buttons & WPAD_CLASSIC_BUTTON_L) {
            conv_buttons |= BCMPAD_BUTTON_L;
        }
        return conv_buttons;
    }

    uint32_t remapProButtons(const uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & WPAD_PRO_BUTTON_LEFT) {
            conv_buttons |= BCMPAD_BUTTON_LEFT;
        }
        if (buttons & WPAD_PRO_BUTTON_RIGHT) {
            conv_buttons |= BCMPAD_BUTTON_RIGHT;
        }
        if (buttons & WPAD_PRO_BUTTON_DOWN) {
            conv_buttons |= BCMPAD_BUTTON_DOWN;
        }
        if (buttons & WPAD_PRO_BUTTON_UP) {
            conv_buttons |= BCMPAD_BUTTON_UP;
        }
        if (buttons & WPAD_PRO_BUTTON_PLUS) {
            conv_buttons |= BCMPAD_BUTTON_PLUS;
        }
        if (buttons & WPAD_PRO_BUTTON_X) {
            conv_buttons |= BCMPAD_BUTTON_X;
        }
        if (buttons & WPAD_PRO_BUTTON_Y) {
            conv_buttons |= BCMPAD_BUTTON_Y;
        }
        if (buttons & WPAD_PRO_BUTTON_B) {
            conv_buttons |= BCMPAD_BUTTON_B;
        }
        if (buttons & WPAD_PRO_BUTTON_A) {
            conv_buttons |= BCMPAD_BUTTON_A;
        }
        if (buttons & WPAD_PRO_BUTTON_MINUS) {
            conv_buttons |= BCMPAD_BUTTON_MINUS;
        }
        if (buttons & WPAD_PRO_TRIGGER_ZR) {
            conv_buttons |= BCMPAD_BUTTON_ZR;
        }
        if (buttons & WPAD_PRO_TRIGGER_ZL) {
            conv_buttons |= BCMPAD_BUTTON_ZL;
        }
        if (buttons & WPAD_PRO_TRIGGER_R) {
            conv_buttons |= BCMPAD_BUTTON_R;
        }
        if (buttons & WPAD_PRO_TRIGGER_L) {
            conv_buttons |= BCMPAD_BUTTON_L;
        }
        if (buttons & WPAD_PRO_BUTTON_STICK_L) {
            conv_buttons |= BCMPAD_BUTTON_STICK_L;
        }
        if (buttons & WPAD_PRO_BUTTON_STICK_R) {
            conv_buttons |= BCMPAD_BUTTON_STICK_R;
        }
        if (buttons & WPAD_PRO_RESERVED) {
            conv_buttons |= BCMPAD_BUTTON_RESERVED_BIT;
        }
        return conv_buttons;
    }

    ButtonComboModule_ControllerTypes convert(const WPADChan chan) {
        switch (chan) {
            case WPAD_CHAN_0:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0;
            case WPAD_CHAN_1:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1;
            case WPAD_CHAN_2:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2;
            case WPAD_CHAN_3:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3;
            case WPAD_CHAN_4:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4;
            case WPAD_CHAN_5:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5;
            case WPAD_CHAN_6:
                return BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6;
        }
        return BUTTON_COMBO_MODULE_CONTROLLER_NONE;
    }

    ButtonComboModule_ControllerTypes convert(const VPADChan chan) {
        switch (chan) {
            case VPAD_CHAN_0:
                return BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0;
            case VPAD_CHAN_1:
                return BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1;
        }
        return BUTTON_COMBO_MODULE_CONTROLLER_NONE;
    }
} // namespace

ButtonComboManager::ButtonComboManager() = default;

std::optional<std::shared_ptr<ButtonComboInfoIF>> ButtonComboManager::CreateComboInfo(const ButtonComboModule_ComboOptions &options, ButtonComboModule_Error &err) {
    if (options.version != BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION) {
        err = BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION;
        return std::nullopt;
    }
    if (options.buttonComboOptions.basicCombo.combo == 0 ||
        options.buttonComboOptions.basicCombo.controllerMask == BUTTON_COMBO_MODULE_CONTROLLER_NONE) {
        err = BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO;
        return std::nullopt;
    }

    bool observer = false;
    switch (options.buttonComboOptions.type) {
        case BUTTON_COMBO_MODULE_TYPE_HOLD_OBSERVER:
            observer = true;
            __attribute__((fallthrough));
        case BUTTON_COMBO_MODULE_TYPE_HOLD: {

            if (options.buttonComboOptions.optionalHoldForXFrames == 0) {
                err = BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING;
                return std::nullopt;
            }
            err = BUTTON_COMBO_MODULE_ERROR_SUCCESS;
            return std::make_shared<ButtonComboInfoHold>(options.metaOptions.label,
                                                         options.buttonComboOptions.basicCombo.controllerMask,
                                                         options.buttonComboOptions.basicCombo.combo,
                                                         options.buttonComboOptions.optionalHoldForXFrames,
                                                         options.callbackOptions.callback,
                                                         options.callbackOptions.context,
                                                         observer);
        }
        case BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN_OBSERVER:
            observer = true;
            __attribute__((fallthrough));
        case BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN: {
            err = BUTTON_COMBO_MODULE_ERROR_SUCCESS;
            return std::make_shared<ButtonComboInfoDown>(options.metaOptions.label,
                                                         options.buttonComboOptions.basicCombo.controllerMask,
                                                         options.buttonComboOptions.basicCombo.combo,
                                                         options.callbackOptions.callback,
                                                         options.callbackOptions.context,
                                                         observer);
        }
        case BUTTON_COMBO_MODULE_TYPE_INVALID:
            break;
    }
    err = BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO_TYPE;
    DEBUG_FUNCTION_LINE_ERR("Failed to create ButtonComboInfo, unsupported type");
    return std::nullopt;
}

bool ButtonComboManager::hasActiveComboWithTVButton() {
    std::lock_guard lock(mMutex);
    return std::ranges::any_of(mCombos, [](const auto &combo) { return combo->getStatus() == BUTTON_COMBO_MODULE_COMBO_STATUS_VALID && combo->getCombo() & BCMPAD_BUTTON_TV; });
}

ButtonComboModule_ComboStatus ButtonComboManager::CheckComboStatus(const ButtonComboInfoIF &other) {
    if (other.isObserver()) {
        return BUTTON_COMBO_MODULE_COMBO_STATUS_VALID;
    }
    std::lock_guard lock(mMutex);
    for (const auto &existingComboInfo : mCombos) {
        // Don't check ourselves
        if (existingComboInfo->getHandle() == other.getHandle()) {
            continue;
        }
        if (existingComboInfo->conflictsWith(other)) {
            return BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT;
        }
    }
    return BUTTON_COMBO_MODULE_COMBO_STATUS_VALID;
}

void ButtonComboManager::AddCombo(std::shared_ptr<ButtonComboInfoIF> newComboInfo, ButtonComboModule_ComboHandle &outHandle, ButtonComboModule_ComboStatus &outStatus) {
    std::lock_guard lock(mMutex);

    outStatus = CheckComboStatus(*newComboInfo);
    newComboInfo->setStatus(outStatus);
    outHandle = newComboInfo->getHandle();
    mCombos.emplace_front(std::move(newComboInfo));

    const auto block = hasActiveComboWithTVButton();
    VPADSetTVMenuInvalid(VPAD_CHAN_0, block);
    VPADSetTVMenuInvalid(VPAD_CHAN_1, block);
}

ButtonComboModule_Error ButtonComboManager::RemoveCombo(ButtonComboModule_ComboHandle handle) {
    std::lock_guard lock(mMutex);
    if (!remove_first_if(mCombos, [handle](const auto &combo) { return combo->getHandle() == handle; })) {
        DEBUG_FUNCTION_LINE_WARN("Failed to remove combo by handle %08X", handle);
    } else {
        const auto block = hasActiveComboWithTVButton();
        VPADSetTVMenuInvalid(VPAD_CHAN_0, block);
        VPADSetTVMenuInvalid(VPAD_CHAN_1, block);
    }

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::GetButtonComboStatus(const ButtonComboModule_ComboHandle handle, ButtonComboModule_ComboStatus &outComboStatus) {
    std::lock_guard lock(mMutex);
    const auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    outComboStatus = comboInfo->getStatus();

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

void ButtonComboManager::UpdateInputVPAD(const VPADChan chan, const VPADStatus *buffer, const uint32_t bufferSize, const VPADReadError *error) {
    if (chan < VPAD_CHAN_0 || chan > VPAD_CHAN_1) {
        DEBUG_FUNCTION_LINE_ERR("Invalid VPADChan");
        return;
    }
    if (buffer == nullptr || !error || *error != VPAD_READ_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Invalid buffer or error state");
        return;
    }

    {
        std::lock_guard lock(mMutex);
        const auto controller   = convert(chan);
        uint32_t usedBufferSize = 1;
        // Fix games like TP HD
        if (VPADGetButtonProcMode(chan) == 1) {
            usedBufferSize = bufferSize;
        }

        if (usedBufferSize > mVPADButtonBuffer.size()) {
            mVPADButtonBuffer.resize(usedBufferSize);
        }

        // the order of the "buffer" data is new -> old, but we want it to be in old -> new
        for (uint32_t i = 0; i < usedBufferSize; i++) {
            mVPADButtonBuffer[usedBufferSize - i - 1] = remapVPADButtons(buffer[i].hold);
        }

        for (const auto &combo : mCombos) {
            if (combo->getStatus() != BUTTON_COMBO_MODULE_COMBO_STATUS_VALID) {
                continue;
            }
            combo->UpdateInput(controller, std::span(mVPADButtonBuffer.data(), usedBufferSize));
        }
    }
}

void ButtonComboManager::UpdateInputWPAD(const WPADChan chan, WPADStatus *data) {
    if (chan < WPAD_CHAN_0 || chan > WPAD_CHAN_6) {
        DEBUG_FUNCTION_LINE_WARN("Invalid WPADChan %d", chan);
        return;
    }
    if (!data || data->error || data->extensionType == 0xFF) {
        DEBUG_FUNCTION_LINE_VERBOSE("Invalid data or state");
        return;
    }
    const auto controller   = convert(chan);
    uint32_t pressedButtons = {};
    switch (data->extensionType) {
        case WPAD_EXT_CORE:
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_MPLUS_NUNCHUK: {
            pressedButtons = remapWiiMoteButtons(data->buttons);
            break;
        }
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC: {
            const auto classic = reinterpret_cast<WPADStatusClassic *>(data);
            pressedButtons     = remapClassicButtons(classic->buttons);
            break;
        }
        case WPAD_EXT_PRO_CONTROLLER: {
            const auto proController = reinterpret_cast<WPADStatusProController *>(data);
            pressedButtons           = remapProButtons(proController->buttons);
            break;
        }
        default:
            return;
    }
    {
        std::lock_guard lock(mMutex);
        for (const auto &combo : mCombos) {
            if (combo->getStatus() != BUTTON_COMBO_MODULE_COMBO_STATUS_VALID) {
                continue;
            }
            combo->UpdateInput(controller, std::span(&pressedButtons, 1));
        }
    }
}

ButtonComboInfoIF *ButtonComboManager::GetComboInfoForHandle(const ButtonComboModule_ComboHandle handle) {
    for (const auto &combo : mCombos) {
        if (combo->getHandle() == handle) {
            return combo.get();
        }
    }
    return nullptr;
}

ButtonComboModule_Error ButtonComboManager::UpdateButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                                  const ButtonComboModule_MetaOptions &options) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    comboInfo->setMetaOptions(options);

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::UpdateButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                      const ButtonComboModule_CallbackOptions &options) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    comboInfo->setCallbackOptions(options);

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::UpdateControllerMask(const ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_ControllerTypes controllerMask,
                                                                 ButtonComboModule_ComboStatus &outComboStatus) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    comboInfo->setControllerMask(controllerMask);

    // check if we have a conflict.
    comboInfo->setControllerMask(controllerMask);
    comboInfo->setStatus(CheckComboStatus(*comboInfo));
    outComboStatus = comboInfo->getStatus();

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::UpdateButtonCombo(const ButtonComboModule_ComboHandle handle,
                                                              ButtonComboModule_Buttons combo,
                                                              ButtonComboModule_ComboStatus &outComboStatus) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    comboInfo->setCombo(combo);
    comboInfo->setStatus(CheckComboStatus(*comboInfo));
    outComboStatus = comboInfo->getStatus();

    const auto block = hasActiveComboWithTVButton();
    VPADSetTVMenuInvalid(VPAD_CHAN_0, block);
    VPADSetTVMenuInvalid(VPAD_CHAN_1, block);

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::UpdateHoldDuration(const ButtonComboModule_ComboHandle handle,
                                                               const uint32_t holdDurationInFrames) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return comboInfo->setHoldDuration(holdDurationInFrames);
}

ButtonComboModule_Error ButtonComboManager::GetButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_MetaOptionsOut &outOptions) {
    std::lock_guard lock(mMutex);
    const auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    if (!comboInfo->getMetaOptions(outOptions)) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::GetButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                   ButtonComboModule_CallbackOptions &outOptions) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    outOptions = comboInfo->getCallbackOptions();

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboManager::GetButtonComboInfoEx(const ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_ButtonComboInfoEx &outOptions) {
    std::lock_guard lock(mMutex);
    auto *comboInfo = GetComboInfoForHandle(handle);
    if (!comboInfo) {
        DEBUG_FUNCTION_LINE_ERR("ButtonComboModule_GetButtonComboInfo failed to get manager for handle %08X", handle);
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    outOptions = comboInfo->getComboInfoEx();

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_ComboStatus ButtonComboManager::CheckComboAvailable(const ButtonComboModule_ButtonComboOptions &other) {
    std::lock_guard lock(mMutex);
    for (const auto &existingComboInfo : mCombos) {
        if (existingComboInfo->conflictsWith(other)) {
            return BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT;
        }
    }
    return BUTTON_COMBO_MODULE_COMBO_STATUS_VALID;
}

ButtonComboModule_Error ButtonComboManager::DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions &options,
                                                                       ButtonComboModule_Buttons &outButtonCombo) {
    std::lock_guard lock(mMutex);

    if (options.controllerMask == BUTTON_COMBO_MODULE_CONTROLLER_NONE) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (options.holdComboForInMs == 0 || options.holdAbortForInMs == 0 || options.abortButtonCombo == 0) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    KPADStatus kpad_data{};
    KPADError kpad_error;

    uint32_t holdFor               = 0;
    uint32_t lastHold              = 0;
    const uint32_t holdForTarget   = options.holdComboForInMs >> 4; // roughly ms to frames. Works because we wait 16ms in the loop
    const uint32_t holdAbortTarget = options.holdAbortForInMs >> 4; // roughly ms to frames. Works because we wait 16ms in the loop
    const uint32_t abortButton     = options.abortButtonCombo;

    while (true) {
        uint32_t buttonsHold = 0;
        for (int i = 0; i < 2; i++) {
            VPADReadError vpad_error = VPAD_READ_UNINITIALIZED;
            VPADStatus vpad_data     = {};
            if (i == 0 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0)) { continue; }
            if (i == 1 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1)) { continue; }
            if (VPADRead(static_cast<VPADChan>(i), &vpad_data, 1, &vpad_error) > 0 && vpad_error == VPAD_READ_SUCCESS) {
                buttonsHold = remapVPADButtons(vpad_data.hold);
            }
        }
        for (int i = 0; i < 7; i++) {
            if (i == 0 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0)) { continue; }
            if (i == 1 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1)) { continue; }
            if (i == 2 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2)) { continue; }
            if (i == 3 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3)) { continue; }
            if (i == 4 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4)) { continue; }
            if (i == 5 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5)) { continue; }
            if (i == 6 && !(options.controllerMask & BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6)) { continue; }
            kpad_data = {};
            if (KPADReadEx(static_cast<KPADChan>(i), &kpad_data, 1, &kpad_error) > 0) {
                if (kpad_error == KPAD_ERROR_OK && kpad_data.extensionType != 0xFF) {
                    if (kpad_data.extensionType == WPAD_EXT_CORE || kpad_data.extensionType == WPAD_EXT_NUNCHUK) {
                        buttonsHold |= remapWiiMoteButtons(kpad_data.hold);
                    } else if (kpad_data.extensionType == WPAD_EXT_PRO_CONTROLLER) {
                        buttonsHold |= remapProButtons(kpad_data.pro.hold);
                    } else {
                        buttonsHold |= remapClassicButtons(kpad_data.classic.hold);
                    }
                }
            }
        }

        if (buttonsHold == lastHold) {
            if (buttonsHold != 0) {
                holdFor++;
            }
        } else {
            holdFor = 0;
        }
        lastHold = buttonsHold;

        if (holdFor >= holdAbortTarget && lastHold == abortButton) {
            DEBUG_FUNCTION_LINE("Aborted button combo detection");
            return BUTTON_COMBO_MODULE_ERROR_ABORTED;
        }

        if (holdFor >= holdForTarget) {
            DEBUG_FUNCTION_LINE_INFO("Detected button combo %08X", lastHold);
            outButtonCombo = static_cast<ButtonComboModule_Buttons>(lastHold);
            break;
        }
        OSSleepTicks(OSMillisecondsToTicks(16));
    }

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}
