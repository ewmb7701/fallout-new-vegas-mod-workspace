#pragma once
#include "GameObjects.h"

// Engine result layout for FalloutNV 1.4.0.525; WaypointPath is our name for it.
class WaypointPath
{
public:
  struct Space { UInt8 exterior; UInt8 padding[3]; TESForm *worldspace; TESForm *cell; };
  struct Transition { TESObjectREFR *door; float arrival[3]; };
  template<class T> struct NativeArray { void *vtable; T *data; UInt32 size, capacity; };

  NativeArray<Space> spaces;
  NativeArray<Transition> transitions;
  float origin[3], destination[3];

  WaypointPath();
  ~WaypointPath();
  WaypointPath(const WaypointPath &) = delete;
  WaypointPath &operator=(const WaypointPath &) = delete;
  bool Find(TESObjectREFR *origin, TESObjectREFR *destination, UInt32 doorAccessMode);
  float Length() const;

};
STATIC_ASSERT(sizeof(WaypointPath) == 0x38);
STATIC_ASSERT(offsetof(WaypointPath, transitions) == 0x10);
STATIC_ASSERT(sizeof(WaypointPath::Space) == 0xC);
STATIC_ASSERT(sizeof(WaypointPath::Transition) == 0x10);
