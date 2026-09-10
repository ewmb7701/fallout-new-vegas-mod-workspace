#pragma once

#include "GameForms.h"

// Runtime layout from nvse/GameObjects.h (FalloutNV 1.4.0.525).
class TESObjectREFR : public TESForm
{
public:
  UInt8 opaqueSlots1[8];
  TESForm *baseForm; // 20
  UInt8 opaqueSlots2[0x68 - 0x24];

};
STATIC_ASSERT(offsetof(TESObjectREFR, baseForm) == 0x20);
STATIC_ASSERT(sizeof(TESObjectREFR) == 0x68);
