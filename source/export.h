#pragma once

#include <buttoncombo/defines.h>

extern ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                                ButtonComboModule_ComboHandle *outHandle,
                                                                ButtonComboModule_ComboStatus *outStatus);

extern ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(const ButtonComboModule_ComboHandle handle);
