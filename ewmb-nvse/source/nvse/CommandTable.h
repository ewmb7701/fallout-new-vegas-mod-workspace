#pragma once

#include "GameForms.h"

enum ParamType
{
  kParamType_Float = 0x02,
  kParamType_ActorValue = 0x05,
  kParamType_ActorBase = 0x19,
};

struct ParamInfo
{
  const char *typeStr;
  UInt32 typeID;
  UInt32 isOptional;
};

#define COMMAND_ARGS ParamInfo *paramInfo, void *scriptData, TESObjectREFR *thisObj, TESObjectREFR *containingObj, Script *scriptObj, ScriptEventList *eventList, double *result, UInt32 *opcodeOffsetPtr
#define EXTRACT_ARGS paramInfo, scriptData, opcodeOffsetPtr, thisObj, containingObj, scriptObj, eventList

using Cmd_Execute = bool (*)(COMMAND_ARGS);

struct CommandInfo
{
  const char *longName;
  const char *shortName;
  UInt32 opcode;
  const char *helpText;
  UInt16 needsParent;
  UInt16 numParams;
  ParamInfo *params;
  Cmd_Execute execute;
  void *parse;
  void *eval;
  UInt32 flags;
};
STATIC_ASSERT(sizeof(CommandInfo) == 0x28);
