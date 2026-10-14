#pragma once

#include "nvse/PluginAPI.h"

class TESObjectREFR;

namespace OnEnemyHealthTargetChange
{
  void RegisterEvent(NVSEEventManagerInterface *eventInterface);
  void InstallHook();
  bool AllowChange(TESObjectREFR *target);
}
