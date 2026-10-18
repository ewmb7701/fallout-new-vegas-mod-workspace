#pragma once

#include "GameForms.h"

// ExtraMapMarker's marker data, as returned by the native reference getter.
struct MapMarkerData
{
  UInt8 opaque00[0xC];
  UInt8 flags; // bit 0: visible; bit 1: fast travel enabled
};
STATIC_ASSERT(offsetof(MapMarkerData, flags) == 0xC);

// Runtime layout from nvse/GameObjects.h (FalloutNV 1.4.0.525).
class TESObjectREFR : public TESForm
{
public:
  UInt8 opaqueSlots1[8];
  TESForm *baseForm; // 20
  UInt8 opaqueSlots2[0x68 - 0x24];

  const float *GetPosition();
  MapMarkerData *GetMapMarkerData();
  bool IsAvailableMapMarker();

};
STATIC_ASSERT(offsetof(TESObjectREFR, baseForm) == 0x20);
STATIC_ASSERT(sizeof(TESObjectREFR) == 0x68);

bool QueueFastTravelToMarker(TESObjectREFR *marker);

// Native player position request; a marker identifies the fast-travel branch.
struct PlayerPositionRequest
{
  UInt8 opaque00[0x30];
  TESObjectREFR *marker;
};
STATIC_ASSERT(offsetof(PlayerPositionRequest, marker) == 0x30);
STATIC_ASSERT(sizeof(PlayerPositionRequest) == 0x34);

PlayerPositionRequest *GetPlayerPositionRequest();
