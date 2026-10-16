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
  kInterface_ArrayVar = 5,
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

// ABI prefix from xNVSE PluginAPI.h. Only outgoing borrowed elements are used.
struct NVSEArrayVarInterface
{
  struct Array;
  struct Element
  {
    union { double number; TESForm *form; const char *string; Array *array; };
    UInt8 type;
    static Element Number(double v) { Element e{}; e.number = v; e.type = 1; return e; }
    static Element Form(TESForm *v) { Element e{}; e.form = v; e.type = 2; return e; }
    static Element String(const char *v) { Element e{}; e.string = v; e.type = 3; return e; }
    static Element ArrayValue(Array *v) { Element e{}; e.array = v; e.type = 4; return e; }
  };
  Array *(*CreateArray)(const Element *, UInt32, Script *);
  Array *(*CreateStringMap)(const char **, const Element *, UInt32, Script *);
  Array *(*CreateMap)(const double *, const Element *, UInt32, Script *);
  bool (*AssignCommandResult)(Array *, double *);
  void (*SetElement)(Array *, const Element &, const Element &);
  void (*AppendElement)(Array *, const Element &);
};
STATIC_ASSERT(offsetof(NVSEArrayVarInterface::Element, type) == 8);
STATIC_ASSERT(sizeof(NVSEArrayVarInterface::Element) == 16);
