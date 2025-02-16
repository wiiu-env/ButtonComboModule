#include <memory>

#include <coreinit/time.h>

class ButtonComboManager;

extern std::unique_ptr<ButtonComboManager> gButtonComboManager;

extern OSTime gTVPressed[2];
extern bool gTVMenuBlocked[2];
