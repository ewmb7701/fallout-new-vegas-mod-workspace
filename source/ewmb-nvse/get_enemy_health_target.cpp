#include "get_enemy_health_target.h"
#include "nvse/GameUI.h"
#include "nvse/GameObjects.h"

namespace
{
  bool ExecuteGetEnemyHealthTarget(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *target = GetEnemyHealthTarget();
    if (target)
      *reinterpret_cast<UInt32 *>(result) = target->refID;
    return true;
  }
}

CommandInfo kCommandInfo_GetEnemyHealthTarget = {
    "GetEnemyHealthTarget", "", 0,
    "Returns the EnemyHealth target, or zero if none", 0,
    0, nullptr, ExecuteGetEnemyHealthTarget, nullptr, nullptr, 0};
