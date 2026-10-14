#include "set_enemy_health_target.h"
#include "on_enemy_health_target_change.h"
#include "nvse/GameAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/GameUI.h"

namespace
{
  bool ExecuteSetEnemyHealthTarget(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *target = nullptr;
    if (!ExtractArgs(EXTRACT_ARGS, &target) || (target && !target->IsActor()))
      return true;

    if (target != GetEnemyHealthTarget() &&
        !OnEnemyHealthTargetChange::AllowChange(target))
      return true;

    *result = SetEnemyHealthTarget(target) ? 1 : 0;
    return true;
  }

  ParamInfo enemyHealthTargetParams[] = {{"actor", kParamType_ObjectRef, 1}};
}

CommandInfo kCommandInfo_SetEnemyHealthTarget = {
    "SetEnemyHealthTarget", "", 0,
    "Sets the EnemyHealth target; omit the actor to clear it", 0,
    1, enemyHealthTargetParams, ExecuteSetEnemyHealthTarget, nullptr, nullptr, 0};
