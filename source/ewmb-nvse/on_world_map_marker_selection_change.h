#pragma once
#include "nvse/PluginAPI.h"
namespace OnWorldMapMarkerSelectionChange
{
  void RegisterEvent(NVSEEventManagerInterface *events);
  void InstallHook();
}
