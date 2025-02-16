#include "ButtonComboManager.h"
#include "function_patches.h"
#include "globals.h"
#include "logger.h"
#include "version.h"

#include <coreinit/cache.h>
#include <coreinit/time.h>
#include <vpad/input.h>

#include <wums.h>

#include <function_patcher/function_patching.h>
#include <buttoncombo/defines.h>

#include <memory>

WUMS_MODULE_EXPORT_NAME("homebrew_buttoncombo");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_DEPENDS_ON(homebrew_functionpatcher);

#define MODULE_VERSION "v0.1.0"

extern ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                                ButtonComboModule_ComboHandle *outHandle,
                                                                ButtonComboModule_ComboStatus *outStatus);

extern ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(const ButtonComboModule_ComboHandle handle);

static ButtonComboModule_ComboHandle sTVButtonHandle;

WUMS_INITIALIZE() {
    initLogging();

    // Now init the library so we can use it for the other patches.
    if (FunctionPatcher_InitLibrary() != FUNCTION_PATCHER_RESULT_SUCCESS) {
        OSFatal("homebrew_buttoncombo: FunctionPatcher_InitLibrary failed");
    }

    bool patchSuccess = false;
    for (uint32_t i = 0; i < function_replacements_size; i++) {
        if (FunctionPatcher_AddFunctionPatch(&function_replacements[i], nullptr, &patchSuccess) != FUNCTION_PATCHER_RESULT_SUCCESS || !patchSuccess) {
            OSFatal("ButtonComboModule: Failed to patch ButtonComboModule function");
        }
    }
    DEBUG_FUNCTION_LINE("Patch ButtonComboModule functions finished");

    gButtonComboManager = std::make_unique<ButtonComboManager>();

    // register observer combo on the TV button
    {
        ButtonComboModule_ComboOptions opt = {};
        opt.version = BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION;
        opt.metaOptions.label = "TV remote overlay combo";
        opt.callbackOptions.callback = [](ButtonComboModule_ControllerTypes triggeredBy,
                                          ButtonComboModule_ComboHandle,
                                          void *)
        {
            VPADChan chan;
            switch (triggeredBy) {
                case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0:
                    chan = VPAD_CHAN_0;
                    break;
                case BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1:
                    chan = VPAD_CHAN_1;
                    break;
                default:
                    return;
            }
            OSReport("TV pressed\n");
            gTVPressed[chan] = OSGetSystemTime();
            OSMemoryBarrier();
        };
        opt.callbackOptions.context = {};
        opt.buttonComboOptions.type = BUTTON_COMBO_MODULE_COMBO_TYPE_PRESS_DOWN_OBSERVER;
        opt.buttonComboOptions.basicCombo.combo = BCMPAD_BUTTON_TV;
        opt.buttonComboOptions.basicCombo.controllerMask = BUTTON_COMBO_MODULE_CONTROLLER_VPAD;
        opt.buttonComboOptions.optionalHoldForXMs = 0;
        if (ButtonComboModule_AddButtonCombo(&opt, &sTVButtonHandle, nullptr) != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
            OSReport("*** FAILED TO SET UP COMBO!\n");
        }
    }

    deinitLogging();
}

WUMS_DEINITIALIZE() {
    ButtonComboModule_RemoveButtonCombo(sTVButtonHandle);
    gButtonComboManager.reset();
}

WUMS_APPLICATION_STARTS() {
    initLogging();
    OSReport("Running ButtonComboModule " MODULE_VERSION MODULE_VERSION_EXTRA "\n");
}

WUMS_APPLICATION_ENDS() {
    deinitLogging();
}
