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
        return true;
    }

    snprintf(outMeta.labelBuffer, outMeta.labelBufferLength, mLabel.c_str());

    return false;
}

void ButtonComboInfoIF::setMetaOptions(ButtonComboModule_MetaOptions options) {
    mLabel = options.label;
}

ButtonComboModule_CallbackOptions ButtonComboInfoIF::getCallbackOptions() const {
    return {.callback = mCallback, .context = mContext};
}

void ButtonComboInfoIF::setCallbackOptions(ButtonComboModule_CallbackOptions options) {
    mCallback = options.callback;
    mContext  = options.context;
}

uint32_t ButtonComboInfoIF::getCombo() const {
    return mCombo;
}
void ButtonComboInfoIF::setCombo(const ButtonComboModule_Buttons combo) {
    mCombo = combo;
}

ButtonComboModule_ComboStatus ButtonComboInfoIF::getStatus() const {
    return mStatus;
}

void ButtonComboInfoIF::setStatus(const ButtonComboModule_ComboStatus status) {
    mStatus = status;
}

ButtonComboModule_ControllerTypes ButtonComboInfoIF::getControllerMask() const {
    return mControllerMask;
}

void ButtonComboInfoIF::setControllerMask(ButtonComboModule_ControllerTypes mask) {
    mControllerMask = mask;
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