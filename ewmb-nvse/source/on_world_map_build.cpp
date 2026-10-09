#include "on_world_map_build.h"
#include "on_world_map_build_hook.h"
#include "utility.h"

namespace
{
  constexpr uintptr_t kNativeWorldMapBuildAddress = 0x79CDB0;
  constexpr char kWorldMapBuildEventName[] = "OnWorldMapBuild";
  using NativeWorldMapBuild = void(__thiscall *)(void *);
  NativeWorldMapBuild nativeWorldMapBuild = nullptr;
  NVSEEventManagerInterface *nvseEventInterface = nullptr;
  void __fastcall BuildWorldMapAndDispatchEvent(void *mapMenu, void *)
  {
    nativeWorldMapBuild(mapMenu);
    // Synchronous: native marker creation/positioning has returned, and the
    // caller (including Keep Pip-Boy Open) has not yet resumed.
    nvseEventInterface->DispatchEvent(kWorldMapBuildEventName, nullptr);
  }

}

void OnWorldMapBuild::RegisterEvent(NVSEEventManagerInterface *eventInterface)
{
  nvseEventInterface = eventInterface;
  nvseEventInterface->RegisterEvent(kWorldMapBuildEventName, 0, nullptr,
                                    NVSEEventManagerInterface::kFlag_FlushOnLoad);
}

void OnWorldMapBuild::InstallHook()
{
  nativeWorldMapBuild = reinterpret_cast<NativeWorldMapBuild>(WorldMapBuildHook::Install(
      kNativeWorldMapBuildAddress, reinterpret_cast<uintptr_t>(&BuildWorldMapAndDispatchEvent)));
  PrintLog("OnWorldMapBuild: post-world-map-build hook installed at %08X", kNativeWorldMapBuildAddress);
}
