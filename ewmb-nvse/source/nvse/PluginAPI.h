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
  bool (*RegisterTypedCommand)(CommandInfo *info, UInt32 returnType);
  const char *(*GetRuntimeDirectory)();
  UInt32 isNogore;
};

struct PluginInfo
{
  enum { kInfoVersion = 1 };
  UInt32 infoVersion;
  const char *name;
  UInt32 version;
};

// ABI prefixes from xNVSE's PluginAPI.h. Only the used members are declared.
// https://github.com/xNVSE/NVSE/blob/master/nvse/nvse/PluginAPI.h
enum
{
  kInterface_Messaging = 2,
  kInterface_EventManager = 8,
};

struct NVSEMessagingInterface
{
  struct Message
  {
    const char *sender;
    UInt32 type;
    UInt32 dataLen;
    void *data;
  };
  enum { kMessage_ExitToMainMenu = 2, kMessage_PreLoadGame = 6,
         kMessage_NewGame = 14, kMessage_DeferredInit = 18 };
  UInt32 version;
  bool (*RegisterListener)(UInt32 listener, const char *sender, void (*handler)(Message *));
  bool (*Dispatch)(UInt32 sender, UInt32 type, void *data, UInt32 dataLen, const char *receiver);
};

struct NVSEEventManagerInterface
{
  // Read-only ABI view of NVSEArrayVarInterface::Element. Never own/free it.
  struct ResultElement
  {
    union { double number; void *pointer; };
    UInt8 type; // 1 = numeric
  };
  using DispatchCallback = bool (*)(ResultElement &, void *);
  enum ParamType : UInt8 { eParamType_Int = 1, eParamType_Reference = 5 };
  enum EventFlags : UInt32 { kFlags_None = 0, kFlag_FlushOnLoad = 1 };
  // This interface starts with function pointers, NOT a version field.
  bool (*RegisterEvent)(const char *name, UInt8 numParams, ParamType *params, EventFlags flags);
  bool (*DispatchEvent)(const char *name, TESObjectREFR *callingRef, ...);
  enum DispatchReturn : signed char { kRetn_Error = -1, kRetn_Normal = 0 };
  DispatchReturn (*DispatchEventAlt)(const char *name, DispatchCallback callback,
                                   void *data, TESObjectREFR *callingRef, ...);
};

STATIC_ASSERT(offsetof(NVSEEventManagerInterface::ResultElement, type) == 8);
