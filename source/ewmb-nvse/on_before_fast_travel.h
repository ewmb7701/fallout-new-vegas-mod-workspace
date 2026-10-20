#pragma once
#include "nvse/PluginAPI.h"

namespace OnBeforeFastTravel
{
  void RegisterEvent(NVSEEventManagerInterface *events);
  void InstallHook();
}
