#include "ButtonComboManager.h"
#include "globals.h"

#include <buttoncombo/defines.h>
#include <logger.h>

#include <wums/exports.h>

ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                         ButtonComboModule_ComboHandle *outHandle,
                                                         ButtonComboModule_ComboStatus *outStatus) {
    if (options == nullptr || outHandle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    ButtonComboModule_Error err = BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    const auto comboInfoMaybe   = ButtonComboManager::CreateComboInfo(*options, err);
    if (!comboInfoMaybe || err != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_WARN("Failed to create combo info for options");
        return err;
    }

    ButtonComboModule_ComboHandle handle;
    ButtonComboModule_ComboStatus tmpStatus = BUTTON_COMBO_MODULE_COMBO_STATUS_INVALID_STATUS;
    gButtonComboManager->AddCombo(*comboInfoMaybe, handle, tmpStatus);
    if (outStatus) { *outStatus = tmpStatus; }
    *outHandle = handle;

    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(const ButtonComboModule_ComboHandle handle) {
    if (handle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->RemoveCombo(handle);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboStatus(const ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ComboStatus *outComboStatus) {
    if (handle == nullptr || outComboStatus == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->GetButtonComboStatus(handle, *outComboStatus);
}

ButtonComboModule_Error ButtonComboModule_UpdateButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                                const ButtonComboModule_MetaOptions *options) {
    if (handle == nullptr || options == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->UpdateButtonComboMeta(handle, *options);
}

ButtonComboModule_Error ButtonComboModule_UpdateButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                    const ButtonComboModule_CallbackOptions *options) {
    if (handle == nullptr || options == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->UpdateButtonComboCallback(handle, *options);
}

ButtonComboModule_Error ButtonComboModule_UpdateControllerMask(const ButtonComboModule_ComboHandle handle,
                                                               const ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_ComboStatus *outComboStatus) {
    if (handle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    ButtonComboModule_ComboStatus tmpStatus = BUTTON_COMBO_MODULE_COMBO_STATUS_INVALID_STATUS;
    auto res                                = gButtonComboManager->UpdateControllerMask(handle, controllerMask, tmpStatus);
    if (outComboStatus) { *outComboStatus = tmpStatus; }
    return res;
}

ButtonComboModule_Error ButtonComboModule_UpdateButtonCombo(const ButtonComboModule_ComboHandle handle,
                                                            const ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboStatus *outComboStatus) {
    if (handle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    ButtonComboModule_ComboStatus tmpStatus = BUTTON_COMBO_MODULE_COMBO_STATUS_INVALID_STATUS;
    auto res                                = gButtonComboManager->UpdateButtonCombo(handle, combo, tmpStatus);
    if (outComboStatus) { *outComboStatus = tmpStatus; }
    return res;
}

ButtonComboModule_Error ButtonComboModule_UpdateHoldDuration(const ButtonComboModule_ComboHandle handle,
                                                             uint32_t holdDurationInFrames) {
    if (handle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->UpdateHoldDuration(handle, holdDurationInFrames);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                             ButtonComboModule_MetaOptionsOut *outOptions) {
    if (handle == nullptr || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->GetButtonComboMeta(handle, *outOptions);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_CallbackOptions *outOptions) {
    if (handle == nullptr || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->GetButtonComboCallback(handle, *outOptions);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboInfoEx(const ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ButtonComboInfoEx *outOptions) {
    if (handle == nullptr || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->GetButtonComboInfoEx(handle, *outOptions);
}

ButtonComboModule_Error ButtonComboModule_CheckComboAvailable(const ButtonComboModule_ButtonComboOptions *options,
                                                              ButtonComboModule_ComboStatus *outStatus) {
    if (options == nullptr || outStatus == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    *outStatus = gButtonComboManager->CheckComboAvailable(*options);
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboModule_DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions *options,
                                                                     ButtonComboModule_Buttons *outButtonCombo) {
    if (options == nullptr || outButtonCombo == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    if (!gButtonComboManager) {
        DEBUG_FUNCTION_LINE_ERR("gButtonComboManager was nullptr");
        return BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR;
    }

    return gButtonComboManager->DetectButtonCombo_Blocking(*options, *outButtonCombo);
}

ButtonComboModule_Error ButtonComboModule_GetVersion(ButtonComboModule_APIVersion *outVersion) {
    if (outVersion == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }
    *outVersion = 1;
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

WUMS_EXPORT_FUNCTION(ButtonComboModule_AddButtonCombo);
WUMS_EXPORT_FUNCTION(ButtonComboModule_RemoveButtonCombo);
WUMS_EXPORT_FUNCTION(ButtonComboModule_GetButtonComboStatus);
WUMS_EXPORT_FUNCTION(ButtonComboModule_UpdateButtonComboMeta);
WUMS_EXPORT_FUNCTION(ButtonComboModule_UpdateButtonComboCallback);
WUMS_EXPORT_FUNCTION(ButtonComboModule_UpdateControllerMask);
WUMS_EXPORT_FUNCTION(ButtonComboModule_UpdateButtonCombo);
WUMS_EXPORT_FUNCTION(ButtonComboModule_UpdateHoldDuration);
WUMS_EXPORT_FUNCTION(ButtonComboModule_GetButtonComboMeta);
WUMS_EXPORT_FUNCTION(ButtonComboModule_GetButtonComboCallback);
WUMS_EXPORT_FUNCTION(ButtonComboModule_GetButtonComboInfoEx);
WUMS_EXPORT_FUNCTION(ButtonComboModule_CheckComboAvailable);
WUMS_EXPORT_FUNCTION(ButtonComboModule_DetectButtonCombo_Blocking);
WUMS_EXPORT_FUNCTION(ButtonComboModule_GetVersion);