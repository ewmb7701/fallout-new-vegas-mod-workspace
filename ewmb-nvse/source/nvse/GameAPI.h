#pragma once

#include "CommandTable.h"

using ExtractArgsFn = bool (*)(ParamInfo *, void *, UInt32 *, TESObjectREFR *, TESObjectREFR *, Script *, ScriptEventList *, ...);

extern const ExtractArgsFn ExtractArgs;
