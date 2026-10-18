#include "travel_location.h"

TravelLocation::TravelLocation(TESObjectREFR *ref)
{
  reinterpret_cast<void *(__thiscall *)(void *, TESObjectREFR *)>(0x6DCD70)(this, ref);
}

TravelLocation::~TravelLocation()
{
  reinterpret_cast<void(__thiscall *)(void *)>(0x4FF7E0)(this);
}
