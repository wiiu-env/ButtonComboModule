#include "DRCAttachCallback.h"
#include "ButtonComboManager.h"
#include "globals.h"
#include "logger.h"

#include <function_patcher/fpatching_defines.h>

#include <map>
#include <mutex>

#include <cstring>

std::mutex gDRCCallbackMutex;
CCRCDCCallbackData gCCRCDCCallbackDataCurrent;
std::map<CCRCDCCallbackDataCallback, void *> gDRCCallbackData;

void DRCAttachDetachCallbackWrapper(CCRCDCCallbackData *data, void *) {
    std::lock_guard<std::mutex> lock(gDRCCallbackMutex);
    // Callbacks get called when added, so we need to save the current "data" and then pass it to newly added callbacks
    memcpy(&gCCRCDCCallbackDataCurrent, data, sizeof(CCRCDCCallbackData));

    // Call all callbacks.
    for (auto &cur : gDRCCallbackData) {
        if (cur.first != nullptr) {
            cur.first(data, cur.second);
        }
    }
}

/**
 * From what I can tell `CCRCDCRegisterUVCAttachCallback` is never used by any .rpl/.rpx
 * Let's add support multiple for multiple callbacks anyway, better safe than sorry.
 */
DECL_FUNCTION(void, CCRCDCRegisterUVCAttachCallback, CCRCDCCallbackDataCallback callback, void *context) {
    std::lock_guard<std::mutex> lock(gDRCCallbackMutex);
    if (callback == nullptr && context == nullptr) { // Delete (all) callbacks.
        real_CCRCDCRegisterUVCAttachCallback(callback, context);
        gDRCCallbackData.clear();
        return;
    }
    if (callback != nullptr) {
        // Add callback
        gDRCCallbackData[callback] = context;
        // Call callback
        callback(&gCCRCDCCallbackDataCurrent, context);
    }
}

function_replacement_data_t drc_function_replacements[] = {
        REPLACE_FUNCTION(CCRCDCRegisterUVCAttachCallback, LIBRARY_NSYSCCR, CCRCDCRegisterUVCAttachCallback),
};

uint32_t drc_function_replacements_size = sizeof(drc_function_replacements) / sizeof(function_replacement_data_t);


void DRCAttachDetachCallback(CCRCDCCallbackData *data, void *) {
    if (data->attached) {
        if (gButtonComboManager) {
            const bool block = gButtonComboManager->hasActiveComboWithTVButton();
            VPADSetTVMenuInvalid(data->chan, block);
        }
    }
}

extern "C" int CCRCDCRegisterUVCAttachCallback(void (*)(CCRCDCCallbackData *, void *), void *);

void InitDRCAttachCallbacks() {
    // Clear existing callbacks
    gDRCCallbackData.clear();

    // Add wrapper function to support multiple callbacks.
    real_CCRCDCRegisterUVCAttachCallback(DRCAttachDetachCallbackWrapper, nullptr);

    // Add the real callback we want to use.
    CCRCDCRegisterUVCAttachCallback(DRCAttachDetachCallback, nullptr);
}