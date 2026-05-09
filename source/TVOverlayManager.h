#pragma once

#include <vpad/input.h>

namespace TVOverlayManager
{

    void RegisterCombo();

    void UnregisterCombo();

    void InitVPAD(VPADChan channel);

    void ResetVPAD(VPADChan channel);

    void UpdateVPAD(VPADChan channel);

    void UpdateBlocking();

} // namespace TVOverlayManager
