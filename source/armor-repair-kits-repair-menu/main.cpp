#include "repair.h"
#include <cstdio>
#include <cstdarg>
#include <share.h>
#include "hooks.h"

struct NVSEInterface {
    uint32_t nvseVersion,runtimeVersion,editorVersion,isEditor;
    void* registerCommand; void* setOpcodeBase;
    void* (*QueryInterface)(uint32_t);
    uint32_t (*GetPluginHandle)();
    void* registerTypedCommand;
    const char* (*GetRuntimeDirectory)();
    uint32_t isNogore;
};
struct PluginInfo { uint32_t infoVersion; const char* name; uint32_t version; };
struct Messaging {
    struct Message { const char* sender; uint32_t type,length; void* data; };
    uint32_t version;
    bool (*RegisterListener)(uint32_t,const char*,void (*)(Message*));
};
static FILE* logFile;
static void Log(const char* format,...) {
    if(!logFile) return;
    va_list args; va_start(args,format); vfprintf(logFile,format,args); va_end(args);
    fputc('\n',logFile); fflush(logFile);
}
static Game::Form* FindArmorKit() {
    auto handler=*reinterpret_cast<void**>(0x11C3F2C);
    auto mod=handler ? Game::This<uint8_t*>(0x462F40,handler,"Armor Repair Kits.esp") : nullptr;
    if(!mod || mod[0x40C]==0xFF) { Log("Armor kit unavailable: Armor Repair Kits.esp is not loaded."); return nullptr; }
    if(*reinterpret_cast<uint32_t*>(mod+0x3E8)&0x700) {
        Log("Armor kit unavailable: expected the normal-format Armor Repair Kits.esp."); return nullptr;
    }
    const uint32_t id=(uint32_t(mod[0x40C])<<24)|0x6566;
    auto kit=Game::Lookup(id);
    if(!kit || kit->type!=0x2F || kit->id!=id) { Log("Armor kit unavailable: ALCH %08X is missing.",id); return nullptr; }
    return kit;
}
static void OnMessage(Messaging::Message* message) {
    static bool attempted=false;
    // DeferredInit (18) precedes Stewie's own WinMain injector. Wait for the
    // first main-loop notification after menus and that injector are ready.
    if(!message || message->type!=20 || attempted) return;
    if(!*reinterpret_cast<void**>(0x11DA368)) return;
    if(*reinterpret_cast<uint8_t*>(0x86B650)==0xE8) {
        HMODULE owner=nullptr;
        auto stewie=GetModuleHandleA("nvse_stewie_tweaks.dll");
        if(stewie && GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(Hooks::BranchTarget(0x86B650)),&owner) && owner==stewie) return;
    }
    attempted=true;
    Repair::armorKit=FindArmorKit();
    if(!Repair::armorKit) { Log("Disabled: armor kit dependency is unavailable; no hooks installed."); return; }
    if(!Repair::Install()) { Log("Disabled: %s",Repair::Failure()); return; }
    Log("Installed armor-only repair-menu hooks. Armor kit %08X. Weapon handling passes through unchanged.",Repair::armorKit->id);
}
extern "C" __declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface* nvse,PluginInfo* info) {
    *info={1,"Armor Repair Kits - Repair Menu",4};
    return !nvse->isEditor && !nvse->isNogore && nvse->nvseVersion>=6 && nvse->runtimeVersion==0x040020D0;
}
extern "C" __declspec(dllexport) bool NVSEPlugin_Load(const NVSEInterface* nvse) {
    char path[MAX_PATH];
    snprintf(path,sizeof(path),"%sData\\NVSE\\Plugins\\armor_repair_kits_repair_menu.log",nvse->GetRuntimeDirectory());
    logFile=_fsopen(path,"w",_SH_DENYNO);
    Log("Armor Repair Kits - Repair Menu v4 (armor only); waiting for completed menu initialization.");
    auto messaging=static_cast<Messaging*>(nvse->QueryInterface(2));
    return messaging && messaging->RegisterListener(nvse->GetPluginHandle(),"NVSE",OnMessage);
}
