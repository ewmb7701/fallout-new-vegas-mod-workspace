#include "get_pc_can_fast_travel_with_message.h"
#include "nvse/GameUI.h"

namespace
{
  bool ExecuteGetPCCanFastTravelWithMessage(COMMAND_ARGS)
  {
    *result = GetPCCanFastTravelWithMessage() ? 1 : 0;
    return true;
  }
}
CommandInfo kCommandInfo_GetPCCanFastTravelWithMessage = {
    "GetPCCanFastTravelWithMessage", "", 0,
    "Checks native player fast-travel eligibility with rejection messages enabled", 0,
    0, nullptr, ExecuteGetPCCanFastTravelWithMessage, nullptr, nullptr, 0};
