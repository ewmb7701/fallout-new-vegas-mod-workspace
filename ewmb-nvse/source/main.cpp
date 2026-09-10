#include "nvse/PluginAPI.h"
#include "utility.h"

#include "ewmb_fn_actor.h"

#define EWMB_VERSION 1

bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info)
{
  info->infoVersion = PluginInfo::kInfoVersion;
  info->name = "ewmb NVSE";
  info->version = EWMB_VERSION;

  if (nvse->isEditor)
    return false;

  CreateLog("Data\\NVSE\\Plugins\\ewmb_nvse.log");
  PrintLog("ewmb_nvse v%u queried (NVSE: 0x%08X, runtime: 0x%08X)",
           EWMB_VERSION, nvse->nvseVersion, nvse->runtimeVersion);

  PrintLog("NVSE version:\t0x%08X\nEWMB version:\t%u\n", nvse->nvseVersion, EWMB_VERSION);

  return true;
}

bool NVSEPlugin_Load(NVSEInterface *nvse)
{
  nvse->SetOpcodeBase(0x7700);

  /*7700*/ nvse->RegisterCommand(&kCommandInfo_SetBaseActorValue);

  PrintLog("ewmb NVSE loaded successfully");

  return true;
}
