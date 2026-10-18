#pragma once

#include "nvse/PluginAPI.h"

namespace OnWorldMapBuild
{
  // Register at plugin load; install the engine hook at DeferredInit.
  void RegisterEvent(NVSEEventManagerInterface *events);
  void InstallHook();
}
