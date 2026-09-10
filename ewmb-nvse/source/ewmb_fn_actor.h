#pragma once

#include "nvse/GameAPI.h"
#include "nvse/GameObjects.h"

#include <cmath>

static ParamInfo kParams_SetBaseActorValue[] = {
    {"actor value", kParamType_ActorValue, 0},
    {"value", kParamType_Float, 0},
    {"actor base", kParamType_ActorBase, 1}};

bool Cmd_SetBaseActorValue_Execute(COMMAND_ARGS)
{
  *result = 0;
  UInt32 actorValue = 0;
  float value = 0;
  TESActorBase *actorBase = nullptr;
  if (!ExtractArgs(EXTRACT_ARGS, &actorValue, &value, &actorBase) ||
      actorValue > 76 || !std::isfinite(value))
    return true;

  if (!actorBase && thisObj && thisObj->IsActor() &&
      thisObj->baseForm && thisObj->baseForm->IsActorBase())
    actorBase = static_cast<TESActorBase *>(thisObj->baseForm);
  if (!actorBase || !actorBase->IsActorBase())
    return true;

  actorBase->SetActorValue(actorValue, value);
  *result = 1;
  return true;
}

static CommandInfo kCommandInfo_SetBaseActorValue = {
    "SetBaseActorValue", "SetBaseAV", 0,
    "Sets an actor base value; returns 1 when accepted, 0 for invalid arguments or target", 0,
    static_cast<UInt16>(sizeof(kParams_SetBaseActorValue) / sizeof(ParamInfo)),
    kParams_SetBaseActorValue, Cmd_SetBaseActorValue_Execute, nullptr, nullptr, 0};
