#pragma once
#include "nvse/PluginAPI.h"

// Signed grid coordinates. Returns {visited: 0/1, expired: -1/0/1}.
// Expired -1 means no usable detach timestamp; a failed query returns Ar_Null.
extern CommandInfo kCommandInfo_GetExteriorCellState;
void RegisterExteriorCellStateArrayInterface(NVSEArrayVarInterface *arrays);
