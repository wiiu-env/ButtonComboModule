#pragma once

#include <vpad/input.h>

void registerTVCombo();

void unregisterTVCombo();

void initTVStatus(VPADChan channel, bool block);

void resetTVStatus(VPADChan channel);

void updateTVStatus(VPADChan channel);
