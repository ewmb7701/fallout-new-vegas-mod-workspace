#include "on_world_map_menu_build.h"
#include "on_world_map_menu_build_hook.h"

namespace
{
  constexpr uintptr_t kNativeWorldMapMenuBuildAddress = 0x79CDB0;
  constexpr char kWorldMapMenuBuildEventName[] = "OnWorldMapMenuBuild";
  using NativeWorldMapMenuBuild = void(__thiscall *)(void *);
  NativeWorldMapMenuBuild nativeWorldMapMenuBuild = nullptr;
  NVSEEventManagerInterface *nvseEventInterface = nullptr;
  void __fastcall BuildWorldMapAndDispatchEvent(void *mapMenu, void *)
  {
    nativeWorldMapMenuBuild(mapMenu);
    // Synchronous: native marker creation/positioning has returned, and the
    // caller (including Keep Pip-Boy Open) has not yet resumed.
    nvseEventInterface->DispatchEvent(kWorldMapMenuBuildEventName, nullptr);
  }

}

void OnWorldMapMenuBuild::RegisterEvent(NVSEEventManagerInterface *eventInterface)
{
  nvseEventInterface = eventInterface;
  nvseEventInterface->RegisterEvent(kWorldMapMenuBuildEventName, 0, nullptr,
                                    NVSEEventManagerInterface::kFlag_FlushOnLoad);
}

void OnWorldMapMenuBuild::InstallHook()
{
  nativeWorldMapMenuBuild = reinterpret_cast<NativeWorldMapMenuBuild>(WorldMapMenuBuildHook::Install(
      kNativeWorldMapMenuBuildAddress, reinterpret_cast<uintptr_t>(&BuildWorldMapAndDispatchEvent)));
}
