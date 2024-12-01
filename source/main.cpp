#include "ButtonComboManager.h"
#include "DRCAttachCallback.h"
#include "function_patches.h"
#include "globals.h"
#include "logger.h"
#include "version.h"

#include <wums.h>

#include <function_patcher/function_patching.h>

#include <memory>

WUMS_MODULE_EXPORT_NAME("homebrew_buttoncombo");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_DEPENDS_ON(homebrew_functionpatcher);

#define MODULE_VERSION "v0.1.0"

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
    for (uint32_t i = 0; i < drc_function_replacements_size; i++) {
        if (FunctionPatcher_AddFunctionPatch(&drc_function_replacements[i], nullptr, &patchSuccess) != FUNCTION_PATCHER_RESULT_SUCCESS || !patchSuccess) {
            OSFatal("ButtonComboModule: Failed to patch ButtonComboModule function");
        }
    }
    DEBUG_FUNCTION_LINE("Patch ButtonComboModule functions finished");


    gButtonComboManager = std::make_unique<ButtonComboManager>();

    deinitLogging();
}

WUMS_DEINITIALIZE() {
    gButtonComboManager.reset();
}

WUMS_APPLICATION_STARTS() {
    OSReport("Running ButtonComboModule " MODULE_VERSION MODULE_VERSION_EXTRA "\n");

    InitDRCAttachCallbacks();
}
