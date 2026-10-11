#include "GameObjects.h"

MapMarkerData *TESObjectREFR::GetMapMarkerData()
{
  return reinterpret_cast<MapMarkerData *(__thiscall *)(TESObjectREFR *)>(0x569060)(this);
}

bool TESObjectREFR::IsAvailableMapMarker()
{
  if (!baseForm || baseForm->refID != 0x10 || (flags & 0x820))
    return false; // MapMarker base form; not deleted or disabled.
  MapMarkerData *data = GetMapMarkerData();
  return data && (data->flags & 3) == 3;
}

PlayerPositionRequest *GetPlayerPositionRequest()
{
  auto *player = *reinterpret_cast<UInt8 **>(0x11DEA3C);
  return player ? *reinterpret_cast<PlayerPositionRequest **>(player + 0x1EC) : nullptr;
}

bool QueueFastTravelToMarker(TESObjectREFR *marker)
{
  auto *player = *reinterpret_cast<UInt8 **>(0x11DEA3C);
  if (!player || !marker || !marker->IsAvailableMapMarker() ||
      *reinterpret_cast<void **>(player + 0x1EC)) return false;
  // MapMenu's accepted-travel continuation (798A00) constructs this same request.
  auto *request = reinterpret_cast<UInt8 *(__cdecl *)(UInt32)>(0x401000)(0x34);
  if (!request) return false;
  reinterpret_cast<void(__thiscall *)(void *)>(0x456540)(request);
  *reinterpret_cast<TESObjectREFR **>(request + 0x30) = marker;
  // Submission takes ownership and may process travel synchronously.
  reinterpret_cast<void(__thiscall *)(void *, void *)>(0x93BE30)(player, request);
  return true;
}
