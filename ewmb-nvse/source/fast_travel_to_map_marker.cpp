#include "fast_travel_to_map_marker.h"
#include "nvse/GameAPI.h"
#include "nvse/GameUI.h"
#include "nvse/GameObjects.h"

namespace
{
  bool ExecuteFastTravelToMapMarker(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *marker = nullptr;
    if (ExtractArgs(EXTRACT_ARGS, &marker) && marker && marker->IsAvailableMapMarker() && CanPlayerFastTravelWithMessage())
      *result = QueueFastTravelToMarker(marker) ? 1 : 0;
    return true;
  }
  ParamInfo params[] = {{"map marker", kParamType_ObjectRef, 0}};
}
CommandInfo kCommandInfo_FastTravelToMapMarker = {
    "FastTravelToMapMarker", "", 0,
    "Queues native fast travel to a marker without a prompt or Pip-Boy changes", 0,
    1, params, ExecuteFastTravelToMapMarker, nullptr, nullptr, 0};
