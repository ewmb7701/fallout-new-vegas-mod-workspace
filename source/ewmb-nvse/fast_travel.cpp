#include "fast_travel.h"
#include "nvse/GameAPI.h"
#include "nvse/GameUI.h"
#include "nvse/GameObjects.h"

namespace
{
  bool ExecuteFastTravel(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *marker = nullptr;
    if (ExtractArgs(EXTRACT_ARGS, &marker) && marker && marker->IsAvailableMapMarker() && GetPCCanFastTravelWithMessage())
      *result = QueueFastTravelToMarker(marker) ? 1 : 0;
    return true;
  }
  ParamInfo params[] = {{"map marker", kParamType_ObjectRef, 0}};
}
CommandInfo kCommandInfo_FastTravel = {
    "FastTravel", "", 0,
    "Queues native fast travel to a marker without a prompt or Pip-Boy changes", 0,
    1, params, ExecuteFastTravel, nullptr, nullptr, 0};
