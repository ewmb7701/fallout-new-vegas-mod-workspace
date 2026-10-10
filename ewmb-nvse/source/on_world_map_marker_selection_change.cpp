#include "on_world_map_marker_selection_change.h"
#include "utility.h"
#include <Windows.h>
#include <cstdint>
#include <cstring>

namespace
{
  constexpr char kEventName[] = "OnWorldMapMarkerSelectionChange";
  NVSEEventManagerInterface *eventInterface = nullptr;

  struct MarkerListNode
  {
    TESObjectREFR *marker;
    MarkerListNode *next;
  };

  TESObjectREFR *GetSelectedMarkerReference(UInt8 *mapMenu)
  {
    void *selectedMarkerTile = *reinterpret_cast<void **>(mapMenu + 0xD4);
    if (!selectedMarkerTile) return nullptr;

    // Same tile-index lookup as MapMenu::GetHoveredMarkerRef in Stewie's source.
    using TraitNameToID = UInt32(__cdecl *)(const char *, UInt32);
    using GetTileFloat = float(__thiscall *)(void *, UInt32);
    const UInt32 markerIndexTrait = reinterpret_cast<TraitNameToID>(0xA00940)("_MarkerIndex", 0xFFFFFFFF);
    const int markerIndex = static_cast<int>(reinterpret_cast<GetTileFloat>(0xA011B0)(selectedMarkerTile, markerIndexTrait));
    auto *markerNode = reinterpret_cast<MarkerListNode *>(mapMenu + 0xD8);
    for (int index = 0; markerNode && index < markerIndex; ++index)
      markerNode = markerNode->next;
    return markerNode ? markerNode->marker : nullptr;
  }

  void __stdcall DispatchSelectionChange(void *mapMenu)
  {
    auto *mapMenuBytes = static_cast<UInt8 *>(mapMenu);
    // The native selection routine also handles the local map.
    if (*reinterpret_cast<UInt32 *>(mapMenuBytes + 0x80) == 0x21)
      eventInterface->DispatchEvent(kEventName, nullptr, GetSelectedMarkerReference(mapMenuBytes));
  }

  // The native comparison at 0x79A001 skips this branch for an unchanged tile.
  __declspec(naked) void OnSelectedMarkerChangedHook()
  {
    __asm {
      mov [ecx + 0xD4], edx // replay native selection store before dispatch
      pushfd
      pushad
      push ecx
      call DispatchSelectionChange
      popad
      popfd
      push 0x79A0BC
      ret
    }
  }

  __declspec(naked) void OnSelectedMarkerClearedHook()
  {
    __asm {
      pushfd
      // Native code also clears an already empty selection on every scan.
      cmp dword ptr [eax + 0xD4], 0
      mov dword ptr [eax + 0xD4], 0 // replay native clear before dispatch
      je noSelection
      pushad
      push eax
      call DispatchSelectionChange
      popad
    noSelection:
      popfd
      push 0x79A10E
      ret
    }
  }

  // Stewie-style relative jump patch; each stub resumes after the whole
  // displaced instruction and preserves the native registers and flags.
  void WriteRelJump(uintptr_t address, uintptr_t callback)
  {
    auto *code = reinterpret_cast<UInt8 *>(address);
    const auto displacement = static_cast<SInt32>(callback - address - 5);
    DWORD oldProtection;
    VirtualProtect(code, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
    code[0] = 0xE9;
    std::memcpy(code + 1, &displacement, sizeof(displacement));
    DWORD ignored;
    VirtualProtect(code, 5, oldProtection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), code, 5);
  }
}

void OnWorldMapMarkerSelectionChange::RegisterEvent(NVSEEventManagerInterface *events)
{
  eventInterface = events;
  static NVSEEventManagerInterface::ParamType params[] = {NVSEEventManagerInterface::eParamType_Reference};
  events->RegisterEvent(kEventName, 1, params,
      NVSEEventManagerInterface::kFlag_FlushOnLoad);
}

void OnWorldMapMarkerSelectionChange::InstallHook()
{
  WriteRelJump(0x79A0B6, reinterpret_cast<uintptr_t>(&OnSelectedMarkerChangedHook));
  WriteRelJump(0x79A104, reinterpret_cast<uintptr_t>(&OnSelectedMarkerClearedHook));
  PrintLog("OnWorldMapMarkerSelectionChange: native selection hooks installed at 0079A0B6 and 0079A104");
}
