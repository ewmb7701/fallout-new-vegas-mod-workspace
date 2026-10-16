#pragma once
#include "GameObjects.h"

// Opaque engine path-search location for FalloutNV 1.4.0.525.
// TravelLocation is our name for this native object; its fields are not mapped.
class TravelLocation
{
public:
  explicit TravelLocation(TESObjectREFR *ref);
  ~TravelLocation();
  TravelLocation(const TravelLocation &) = delete;
  TravelLocation &operator=(const TravelLocation &) = delete;

private:
  alignas(4) UInt8 data[0x28];
};
STATIC_ASSERT(sizeof(TravelLocation) == 0x28);
STATIC_ASSERT(alignof(TravelLocation) == 4);
