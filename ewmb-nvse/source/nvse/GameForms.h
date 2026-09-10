#pragma once

#include <cstddef>

class TESForm;
class TESObjectREFR;
class Script;
struct ScriptEventList;
class TESActorBase;

enum FormType : UInt8
{
  kFormType_TESNPC = 0x2A,
  kFormType_TESCreature = 0x2B,
  kFormType_Character = 0x3B,
  kFormType_Creature = 0x3C,
};

// JIP-LN-NVSE 5a30ac4: TESForm virtual methods at their Win32 byte offsets.
struct TESFormVTable
{
  const void *opaque1[0x3E];
  bool(__thiscall *IsActorBase)(TESForm *);
  const void *opaque2; // 0x0FC: IsMobileObject
  bool(__thiscall *IsActor)(TESForm *);
};
STATIC_ASSERT(offsetof(TESFormVTable, IsActorBase) == 0x0F8);
STATIC_ASSERT(offsetof(TESFormVTable, IsActor) == 0x100);

class TESForm
{
public:
  const void *vtable;        // 00
  UInt8 typeID;              // 04
  UInt8 opaque[0x18 - 0x05]; // Unused fields; preserve derived-class offsets.

  bool IsActorBase()
  {
    return static_cast<const TESFormVTable *>(vtable)->IsActorBase(this);
  }

  bool IsActor()
  {
    return static_cast<const TESFormVTable *>(vtable)->IsActor(this);
  }
};
STATIC_ASSERT(sizeof(TESForm) == 0x18);
STATIC_ASSERT(offsetof(TESForm, typeID) == 0x04);
STATIC_ASSERT(offsetof(TESForm, vtable) == 0x00);

// Explicit ABI table: retain unused slots without inventing C++ virtual
// signatures or importing the entire engine class hierarchy.
struct TESActorBaseVTable
{
  const void *opaque[0x64];
  void(__thiscall *SetActorValue)(TESActorBase *, UInt32, float);
};
STATIC_ASSERT(offsetof(TESActorBaseVTable, SetActorValue) == 0x190);

class TESActorBase : public TESForm
{
public:
  UInt8 opaque[0x10C - 0x18];

  void SetActorValue(UInt32 actorValue, float value)
  {
    static_cast<const TESActorBaseVTable *>(vtable)->SetActorValue(this, actorValue, value);
  }
};
STATIC_ASSERT(sizeof(TESActorBase) == 0x10C);
