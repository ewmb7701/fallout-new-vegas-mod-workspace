#include "nvse/PluginAPI.h"
#include "utility.h"

#include "ewmb_fn_actor.h"

#include "on_world_map_build.h"

#define EWMB_VERSION 3

namespace
{
  void HandleNVSEMessage(NVSEMessagingInterface::Message *message)
  {
    if (message->type == NVSEMessagingInterface::kMessage_DeferredInit)
      OnWorldMapBuild::InstallHook();
  }
}

bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info)
{
  info->infoVersion = PluginInfo::kInfoVersion;
  info->name = "ewmb NVSE";
  info->version = EWMB_VERSION;

  if (nvse->isEditor || nvse->nvseVersion < 6 || nvse->isNogore)
    return false;

  CreateLog("Data\\NVSE\\Plugins\\ewmb_nvse.log");
  PrintLog("ewmb_nvse v%u queried (NVSE: 0x%08X, runtime: 0x%08X)",
           EWMB_VERSION, nvse->nvseVersion, nvse->runtimeVersion);

  PrintLog("NVSE version:\t0x%08X\nEWMB version:\t%u\n", nvse->nvseVersion, EWMB_VERSION);

  return true;
}

bool NVSEPlugin_Load(NVSEInterface *nvse)
{
  auto *messaging = static_cast<NVSEMessagingInterface *>(nvse->QueryInterface(kInterface_Messaging));
  auto *events = static_cast<NVSEEventManagerInterface *>(nvse->QueryInterface(kInterface_EventManager));
  OnWorldMapBuild::RegisterEvent(events);
  messaging->RegisterListener(nvse->GetPluginHandle(), "NVSE", HandleNVSEMessage);

  nvse->SetOpcodeBase(0x4318);

  /*0x4318*/ nvse->RegisterCommand(&kCommandInfo_SetBaseActorValue);

  PrintLog("ewmb NVSE loaded successfully");

  return true;
}
