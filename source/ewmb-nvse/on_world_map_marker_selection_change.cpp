#include "on_world_map_marker_selection_change.h"
#include "utility.h"
#include "nvse/GameUI.h"
#include <cstdint>

namespace
{
  constexpr char kEventName[] = "OnWorldMapMarkerSelectionChange";
  NVSEEventManagerInterface *eventInterface = nullptr;

  void __stdcall DispatchSelectionChange(MapMenu *mapMenu)
  {
    // The native selection routine also handles the local map.
    if (mapMenu->IsWorldMap())
      eventInterface->DispatchEvent(kEventName, nullptr, mapMenu->GetSelectedMarker());
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
}
