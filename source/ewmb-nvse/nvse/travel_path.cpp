#include "travel_path.h"
#include "travel_location.h"
#include <cmath>
#include <cstring>
#include <limits>
namespace
{
  bool Valid(TESObjectREFR *ref) { return ref && !(ref->flags & 0x20); }

}
const TravelPath::Teleport *TravelPath::GetTeleport(TESObjectREFR *door)
{
  // Calling the getter on non-doors safely returns null (ExtraTeleport lookup).
  if (!Valid(door)) return nullptr;
  return reinterpret_cast<const Teleport *(__thiscall *)(TESObjectREFR *)>(0x568E50)(door);
}
TravelPath::TravelPath()
{
  reinterpret_cast<void *(__thiscall *)(void *)>(0x6F48B0)(this);
}
TravelPath::~TravelPath()
{
  reinterpret_cast<void(__thiscall *)(void *)>(0x6F4930)(this);
}
bool TravelPath::Find(TESObjectREFR *origin, TESObjectREFR *destination)
{
  auto *player = *reinterpret_cast<void **>(0x11DEA3C);
  if (!player || !Valid(origin) || !Valid(destination)) return false;
  alignas(4) UInt8 actorData[0xC] = {};
  reinterpret_cast<void *(__thiscall *)(void *, void *)>(0x502670)(actorData, player);
  TravelLocation start(origin), end(destination);
  // Same search used by 6D4EB0, retaining its result instead of discarding it.
  if (!reinterpret_cast<bool(__cdecl *)(void *, void *, void *, int, float, void *, int)>(0x6D4D20)(
      &start, &end, this, 0, 0.0f, actorData, 0)) return false;
  return spaces.size && spaces.data && transitions.size == spaces.size - 1 &&
      (!transitions.size || transitions.data);
}
float TravelPath::Distance() const
{
  const float distance = reinterpret_cast<float(__thiscall *)(const void *)>(0x6F49C0)(this);
  return std::isfinite(distance) && distance >= 0 && distance != (std::numeric_limits<float>::max)() ? distance : -1;
}
