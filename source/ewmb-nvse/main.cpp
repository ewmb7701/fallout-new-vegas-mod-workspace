#include "nvse/PluginAPI.h"
#include "utility.h"

#include "ewmb_fn_actor.h"
#include "on_fast_travel_begin.h"
#include "fast_travel.h"
#include "set_pending_fast_travel_marker.h"
#include "on_enemy_health_target_change.h"
#include "set_enemy_health_target.h"
#include "get_enemy_health_target.h"
#include "get_path.h"
#include "is_cell_visited_ex.h"
#include "is_cell_expired_ex.h"
#include "get_exterior_cell_ex.h"
#include "fast_travel_ex.h"
#include "can_player_fast_travel_with_message.h"

#include "on_world_map_build.h"
#include "on_world_map_marker_selection_change.h"

#define EWMB_VERSION 8

namespace
{
  void HandleNVSEMessage(NVSEMessagingInterface::Message *message)
  {
    if (message->type == NVSEMessagingInterface::kMessage_DeferredInit)
    {
      OnWorldMapBuild::InstallHook();
      OnWorldMapMarkerSelectionChange::InstallHook();
      OnFastTravelBegin::InstallHook();
      OnEnemyHealthTargetChange::InstallHook();
    }
  }
}

bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info)
{
  info->infoVersion = PluginInfo::kInfoVersion;
  info->name = "ewmb NVSE";
  info->version = EWMB_VERSION;

  if (nvse->isEditor || nvse->nvseVersion < 6 || nvse->isNogore || nvse->runtimeVersion != 0x040020D0)
    return false;

  CreateLog("Data\\NVSE\\Plugins\\ewmb_nvse.log");
  PrintLog("ewmb_nvse v%u queried (NVSE: 0x%08X, runtime: 0x%08X)",
           EWMB_VERSION, nvse->nvseVersion, nvse->runtimeVersion);

  PrintLog("NVSE version:\t0x%08X\nEWMB version:\t%u\n", nvse->nvseVersion, EWMB_VERSION);

  return true;
}

bool NVSEPlugin_Load(NVSEInterface *nvse)
{
  auto *arrays = static_cast<NVSEArrayVarInterface *>(nvse->QueryInterface(kInterface_ArrayVar));
  if (!arrays)
    return false;
  RegisterPathArrayInterface(arrays);
  auto *messaging = static_cast<NVSEMessagingInterface *>(nvse->QueryInterface(kInterface_Messaging));
  auto *events = static_cast<NVSEEventManagerInterface *>(nvse->QueryInterface(kInterface_EventManager));
  OnWorldMapBuild::RegisterEvent(events);
  OnWorldMapMarkerSelectionChange::RegisterEvent(events);
  OnFastTravelBegin::RegisterEvent(events);
  OnEnemyHealthTargetChange::RegisterEvent(events);
  messaging->RegisterListener(nvse->GetPluginHandle(), "NVSE", HandleNVSEMessage);

  nvse->SetOpcodeBase(0x4318);

  /*0x4318*/ nvse->RegisterCommand(&kCommandInfo_SetBaseActorValue);
  /*0x4319*/ nvse->RegisterCommand(&kCommandInfo_FastTravel);
  /*0x431A*/ nvse->RegisterCommand(&kCommandInfo_SetPendingFastTravelMarker);
  /*0x431B*/ nvse->RegisterCommand(&kCommandInfo_SetEnemyHealthTarget);
  /*0x431C*/ nvse->RegisterTypedCommand(&kCommandInfo_GetEnemyHealthTarget, kRetnType_Form);
  /*0x431D*/ nvse->RegisterTypedCommand(&kCommandInfo_GetPath, kRetnType_Array);
  /*0x431E*/ nvse->RegisterCommand(&kCommandInfo_FastTravelEx);
  /*0x431F*/ nvse->RegisterCommand(&kCommandInfo_IsCellVisitedEx);
  /*0x4320*/ nvse->RegisterCommand(&kCommandInfo_IsCellExpiredEx);
  /*0x4321*/ nvse->RegisterTypedCommand(&kCommandInfo_GetExteriorCellEx, kRetnType_Form);

  /*0x4322*/ nvse->RegisterCommand(&kCommandInfo_CanPlayerFastTravelWithMessage);

  PrintLog("ewmb NVSE loaded successfully");

  return true;
}
