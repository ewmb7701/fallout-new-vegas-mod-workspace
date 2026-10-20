#include "set_pending_fast_travel_marker.h"
#include "nvse/GameAPI.h"
#include "nvse/GameUI.h"
#include "nvse/GameObjects.h"

namespace
{
  bool ExecuteSetPendingFastTravelMarker(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *marker = nullptr;
    if (ExtractArgs(EXTRACT_ARGS, &marker) &&
        marker && marker->IsAvailableMapMarker() && GetPCCanFastTravelWithMessage())
    {
      auto *request = GetPlayerPositionRequest();
      if (request && request->marker)
      {
        request->marker = marker;
        *result = 1;
      }
    }
    return true;
  }
  ParamInfo pendingMarkerParams[] = {{"map marker", kParamType_ObjectRef, 0}};
}
CommandInfo kCommandInfo_SetPendingFastTravelMarker = {
    "SetPendingFastTravelMarker", "", 0,
    "Replaces the marker in the player's pending fast-travel request", 0,
    1, pendingMarkerParams, ExecuteSetPendingFastTravelMarker, nullptr, nullptr, 0};
