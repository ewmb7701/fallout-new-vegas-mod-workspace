#pragma once

#include "CommandTable.h"

struct NVSEInterface
{
  UInt32 nvseVersion;
  UInt32 runtimeVersion;
  UInt32 editorVersion;
  UInt32 isEditor;
  bool (*RegisterCommand)(CommandInfo *info);
  void (*SetOpcodeBase)(UInt32 opcode);
  void *(*QueryInterface)(UInt32 id);
  UInt32 (*GetPluginHandle)();
};

struct PluginInfo
{
  enum { kInfoVersion = 1 };
  UInt32 infoVersion;
  const char *name;
  UInt32 version;
};
