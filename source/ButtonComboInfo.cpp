#include "ButtonComboInfo.h"

#include <logger.h>

ButtonComboInfoIF::ButtonComboInfoIF(std::string label,
                                     const ButtonComboModule_ControllerTypes controllerMask,
                                     const ButtonComboModule_Buttons combo,
                                     const ButtonComboModule_ComboCallback callback,
                                     void *context,
                                     const bool observer) : mLabel(std::move(label)),
                                                            mControllerMask(controllerMask),
                                                            mCombo(combo),
                                                            mCallback(callback),
                                                            mContext(context),
                                                            mIsObserver(observer) {
    // Abuse this as a stable handle that references itself and survives std::move
    *mHandle = reinterpret_cast<uint32_t>(mHandle.get());
}
ButtonComboInfoIF::~ButtonComboInfoIF() = default;

bool ButtonComboInfoIF::isObserver() const {
    return mIsObserver;
}

ButtonComboModule_ComboHandle ButtonComboInfoIF::getHandle() const {
    return ButtonComboModule_ComboHandle((void *) *mHandle);
}

const std::string &ButtonComboInfoIF::getLabel() const {
    return mLabel;
}

bool ButtonComboInfoIF::getMetaOptions(const ButtonComboModule_MetaOptionsOut &outMeta) const {
    if (outMeta.labelBuffer == nullptr || outMeta.labelBufferLength == 0) {
        return false;
    }

    snprintf(outMeta.labelBuffer, outMeta.labelBufferLength, mLabel.c_str());

    return true;
}

void ButtonComboInfoIF::setMetaOptions(const ButtonComboModule_MetaOptions options) {
    mLabel = options.label;
    DEBUG_FUNCTION_LINE("Updated label to: \"%s\", for %08X", mLabel.c_str(), getHandle().handle);
}

ButtonComboModule_CallbackOptions ButtonComboInfoIF::getCallbackOptions() const {
    return {.callback = mCallback, .context = mContext};
}

void ButtonComboInfoIF::setCallbackOptions(const ButtonComboModule_CallbackOptions options) {
    mCallback = options.callback;
    mContext  = options.context;
    DEBUG_FUNCTION_LINE("Updated callback to: %08X(%08X), for %s %08X", mCallback, mContext, mLabel.c_str(), getHandle().handle);
}

uint32_t ButtonComboInfoIF::getCombo() const {
    return mCombo;
}
void ButtonComboInfoIF::setCombo(const ButtonComboModule_Buttons combo) {
    mCombo = combo;
    DEBUG_FUNCTION_LINE("Updated combo to: %08X, for %s %08X", mCombo, mLabel.c_str(), getHandle().handle);
    resetPrevInput();
}

ButtonComboModule_ComboStatus ButtonComboInfoIF::getStatus() const {
    return mStatus;
}

void ButtonComboInfoIF::setStatus(const ButtonComboModule_ComboStatus status) {
    mStatus = status;
    DEBUG_FUNCTION_LINE("Updated status to: %08X, for %s %08X", mStatus, mLabel.c_str(), getHandle().handle);
}

ButtonComboModule_ControllerTypes ButtonComboInfoIF::getControllerMask() const {
    return mControllerMask;
}

void ButtonComboInfoIF::setControllerMask(const ButtonComboModule_ControllerTypes mask) {
    mControllerMask = mask;
    DEBUG_FUNCTION_LINE("Updated controllerMask to: %08X, for %s %08X", mControllerMask, mLabel.c_str(), getHandle().handle);
    resetPrevInput();
}

bool ButtonComboInfoIF::conflictsWith(const ButtonComboModule_ButtonComboOptions &other) const {
    if (mIsObserver) {
        return false;
    }

    // No conflict if this combo is not valid
    if (mStatus != BUTTON_COMBO_MODULE_COMBO_STATUS_VALID) {
        return false;
    }

    // Don't check combos that don't share any target controllers
    if ((mControllerMask & other.controllerMask) == 0) {
        return false;
    }
    if ((other.combo & mCombo) == mCombo || (other.combo & mCombo) == other.combo) {
        return true;
    }
    return false;
}

bool ButtonComboInfoIF::conflictsWith(const ButtonComboInfoIF &other) const {
    return conflictsWith({other.mControllerMask, other.mCombo});
}

int32_t ButtonComboInfoIF::ControllerTypeToChanIndex(const ButtonComboModule_ControllerTypes type) {
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