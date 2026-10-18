#pragma once
#include "GameObjects.h"

// Engine result layout for FalloutNV 1.4.0.525; TravelPath is our name for it.
class TravelPath
{
public:
  struct Space { UInt32 exterior; TESForm *worldspace; TESForm *cell; };
  struct Transition { TESObjectREFR *door; float arrival[3]; };
  template<class T> struct NativeArray { void *vtable; T *data; UInt32 size, capacity; };

  NativeArray<Space> spaces;
  NativeArray<Transition> transitions;
  float origin[3], destination[3];

  TravelPath();
  ~TravelPath();
  TravelPath(const TravelPath &) = delete;
  TravelPath &operator=(const TravelPath &) = delete;
  bool Find(TESObjectREFR *origin, TESObjectREFR *destination);
  float Distance() const;

  struct Teleport { TESObjectREFR *linkedDoor; float position[3]; float rotation[3]; };
  static const Teleport *GetTeleport(TESObjectREFR *door);
};
STATIC_ASSERT(sizeof(TravelPath) == 0x38);
STATIC_ASSERT(offsetof(TravelPath, transitions) == 0x10);
STATIC_ASSERT(sizeof(TravelPath::Space) == 0xC);
STATIC_ASSERT(sizeof(TravelPath::Transition) == 0x10);
