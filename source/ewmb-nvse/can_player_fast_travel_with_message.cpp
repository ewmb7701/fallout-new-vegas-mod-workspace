#include "can_player_fast_travel_with_message.h"
#include "nvse/GameUI.h"

namespace
{
  bool ExecuteCanPlayerFastTravelWithMessage(COMMAND_ARGS)
  {
    *result = CanPlayerFastTravelWithMessage() ? 1 : 0;
    return true;
  }
}
CommandInfo kCommandInfo_CanPlayerFastTravelWithMessage = {
    "CanPlayerFastTravelWithMessage", "", 0,
    "Checks native player fast-travel eligibility with rejection messages enabled", 0,
    0, nullptr, ExecuteCanPlayerFastTravelWithMessage, nullptr, nullptr, 0};
