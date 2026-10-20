#include "waypoint_path.h"
#include "travel_location.h"
#include <cmath>
#include <cstring>
#include <limits>
namespace
{
  bool Valid(TESObjectREFR *ref) { return ref && !(ref->flags & 0x20); }

}
WaypointPath::WaypointPath()
{
  reinterpret_cast<void *(__thiscall *)(void *)>(0x6F48B0)(this);
}
WaypointPath::~WaypointPath()
{
  reinterpret_cast<void(__thiscall *)(void *)>(0x6F4930)(this);
}
bool WaypointPath::Find(TESObjectREFR *origin, TESObjectREFR *destination, UInt32 doorAccessMode)
{
  if (doorAccessMode > 2) return false;
  auto *player = *reinterpret_cast<void **>(0x11DEA3C);
  if (!player || !Valid(origin) || !Valid(destination)) return false;
  alignas(4) UInt8 actorData[0xC] = {};
  reinterpret_cast<void *(__thiscall *)(void *, void *)>(0x502670)(actorData, player);
  TravelLocation start(origin), end(destination);
  // Match the native marker-path builder at 0x952D60: navigation search,
  // then its built-in coarse fallback. Calling 0x6D4D20 directly skips the
  // navigation search and can choose different teleport doors.
  if (!reinterpret_cast<bool(__cdecl *)(void *, void *, void *, void *, int)>(0x6D4F70)(
      &start, &end, this, actorData, doorAccessMode)) return false;
  return spaces.size && spaces.data && transitions.size == spaces.size - 1 &&
      (!transitions.size || transitions.data);
}
float WaypointPath::Length() const
{
  const float length = reinterpret_cast<float(__thiscall *)(const void *)>(0x6F49C0)(this);
  return std::isfinite(length) && length >= 0 && length != (std::numeric_limits<float>::max)() ? length : -1;
}
