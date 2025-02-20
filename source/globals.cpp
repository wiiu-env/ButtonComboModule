#include "globals.h"
#include "ButtonComboManager.h"

std::unique_ptr<ButtonComboManager> gButtonComboManager = {};

OSTime gTVPressed[2];
bool gTVMenuBlocked[2];
