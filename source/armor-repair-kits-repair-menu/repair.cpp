#include "repair.h"
#include "hooks.h"
#include <cstdio>

namespace Repair {
using namespace Game;
Form* armorKit=nullptr;
static uintptr_t originalInit,originalButton,originalPopulate,originalCanClick;
static uintptr_t originalRemove,originalSetup,previousRepair,previousAutoClose,previousSort;
static char failure[128];
const char* Failure() { return failure; }
Form* KitFor(Entry* entry) {
    if(!entry || !entry->form) return nullptr;
    if(entry->form->type==0x18) return armorKit;
    return nullptr;
}
static Entry* Target() { return *repairTarget ? *repairTarget : *inventoryTarget; }
static bool __cdecl HandlesTarget() { return KitFor(Target())!=nullptr; }
bool __stdcall IsKit(Form* form) { return form && form==KitFor(Target()); }
static bool Valid() { auto target=Target(); return KitFor(target) && target->Health()<99.0f; }
static float Fraction() { return RepairFraction(RepairSkill(),*reinterpret_cast<uint8_t*>(0x7818D3)!=0x7A); }
static float __cdecl RepairAmount() {
    auto target=*repairTarget;
    return target && target->form ? RepairedCondition(target->Health(),Fraction()) : 0.0f;
}
static bool CanRepairNormally(Entry* entry) {
    return entry && reinterpret_cast<bool(__stdcall*)(Entry*)>(Hooks::BranchTarget(0x781783))(entry);
}
static Tile* AddKit() {
    auto menu=Menu();
    if(!menu || !Valid()) return nullptr;
    auto kit=KitFor(Target()); const int count=Count(kit);
    if(count<=0) return nullptr;
    char label[256];
    if(count>1) snprintf(label,sizeof(label),"%s (%d)",KitName(kit),count);
    else snprintf(label,sizeof(label),"%s",KitName(kit));
    auto entry=Entry::Create(kit,count);
    auto tile=menu->items.Insert(entry,label);
    if(!tile) { Free(entry->extra); Free(entry); return nullptr; }
    tile->Set(0x1004,RepairAmount());
    if(auto bar=tile->Child(15)) bar->Set(0xFB1,60.0f*Fraction());
    return tile;
}
static void WriteByte(uintptr_t address,uint8_t value) {
    DWORD old;
    if(VirtualProtect(reinterpret_cast<void*>(address),1,PAGE_EXECUTE_READWRITE,&old)) {
        *reinterpret_cast<uint8_t*>(address)=value;
        DWORD ignored; VirtualProtect(reinterpret_cast<void*>(address),1,old,&ignored);
        FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(address),1);
    }
}
static void __cdecl InitList() {
    if(!HandlesTarget()) { Call(originalInit); return; }
    if(auto hovered=*inventoryTarget) {
        const bool normal=CanRepairNormally(hovered);
        // Preserve the game's donor restrictions when opening with kits only.
        if(!normal) { WriteByte(0x7B6C2C,0x9B); WriteByte(0x7B6C82,0x33); }
        Call(originalInit);
        if(!normal) { WriteByte(0x7B6C2C,0xA0); WriteByte(0x7B6C82,0x45); }
    }
    AddKit();
}
static void __fastcall RemoveTile(ItemList* items,void*,Tile* tile) {
    if(!HandlesTarget()) { This(originalRemove,items,tile); return; }
    items->SaveScroll();
    auto selected=items->Selected();
    const bool usedKit=selected && IsKit(selected->form);
    items->Remove(tile);
    if(!usedKit) {
        if(armorKit) items->RemoveForm(armorKit);
    }
    AddKit(); items->Sort(); items->RestoreScroll();
}
static void __fastcall EnableButton(Tile* tile,void*,uint32_t trait,bool enabled) {
    This(originalButton,tile,trait,enabled);
    if(!HandlesTarget()) return;
    if(!tile->Get(trait) && Valid() && Count(KitFor(Target()))>0) {
        tile->Text("Repair Kit"); tile->Set(trait,1);
    }
}
static bool __cdecl KeepOpen() {
    if(!Valid()) return false;
    if(Count(KitFor(Target()))>0) return true;
    auto menu=Menu();
    return menu && This<int>(0x71AE60,&menu->items)>1;
}
static bool __fastcall CanClick(Tile* fixed,void*,int trait) {
    if(!This<bool>(originalCanClick,fixed,trait)) return false;
    if(!HandlesTarget()) return true;
    return fixed->Get(0x1004)-Menu()->brokenHealth->Get(0x1004)>0.001f;
}
static void __fastcall Populate(void* changes,void*,Form* form,Node<Entry>* entries) {
    This(originalPopulate,changes,form,entries);
    if(!HandlesTarget()) return;
    auto selected=*repairTarget;
    if(!CanRepairNormally(selected)) entries->Clear();
    for(auto node=entries; node; node=node->next)
        if(node->data && node->data->form==form) return;
    if(selected) entries->Insert(selected);
}
static int __stdcall CompareKits(ListItem* first,ListItem* second) {
    bool a=first && first->entry && IsKit(first->entry->form);
    bool b=second && second->entry && IsKit(second->entry->form);
    return a||b ? (a==b ? 0 : a ? 1 : -1) : 2;
}
__declspec(naked) static void RepairHook() {
    __asm {
        mov edx, [ebp+8]
        mov eax, [edx+8]
        pushad
        push eax
        call IsKit
        test al, al
        popad
        jnz kit
        cmp previousRepair, 0
        jne passthrough
        fldz
        fstp dword ptr [ebp-14h]
        ret
    passthrough:
        jmp dword ptr [previousRepair]
    kit:
        pop eax
        push 07B5E08h
        jmp RepairAmount
    }
}
__declspec(naked) static void SetupTileHook() {
    __asm {
        mov eax, [ecx+8]
        pushad
        push eax
        call IsKit
        test al, al
        popad
        jnz kit
        jmp dword ptr [originalSetup]
    kit:
        pop eax
        pop eax
        mov eax, 07B5940h
        jmp eax
    }
}
__declspec(naked) static void AutoCloseHook() {
    __asm {
        pushad
        call HandlesTarget
        test al, al
        popad
        jnz armor
        cmp previousAutoClose, 0
        jne passthrough
        jmp closeMenu
    armor:
        pushad
        call KeepOpen
        test al, al
        popad
        jnz keep
    closeMenu:
        mov edx, [ecx]
        mov ecx, [ebp-28h]
        ret
    passthrough:
        jmp dword ptr [previousAutoClose]
    keep:
        pop eax
        pop eax
        mov eax, 07B5D1Ch
        jmp eax
    }
}
__declspec(naked) static void SortHook() {
    __asm {
        push dword ptr [ebp+0Ch]
        push dword ptr [ebp+8]
        call CompareKits
        cmp eax, 2
        je normal
        mov esp, ebp
        pop ebp
        ret
    normal:
        cmp previousSort, 0
        jne passthrough
        mov dword ptr [ebp-8], 0
        mov eax, 07B595Dh
        jmp eax
    passthrough:
        jmp dword ptr [previousSort]
    }
}
bool Install() {
    using namespace Hooks;
    // Shared game hook sites must be vanilla or belong to Stewie. Chain its
    // existing handlers for weapons without modifying its DLL or kit logic.
    std::vector<Patch> patches={
        {0x7B5956,reinterpret_cast<uintptr_t>(SortHook),{0xC7,0x45,0xF8,0,0,0,0},{255,255,255,255,255,255,255},7,0xE9,nullptr},
        {0x7B72F0,reinterpret_cast<uintptr_t>(InitList),{0xE8},{255},5,0xE8,&originalInit},
        {0x7B5CD8,reinterpret_cast<uintptr_t>(RemoveTile),{0xE8},{255},5,0xE8,&originalRemove},
        {0x7B5DA8,reinterpret_cast<uintptr_t>(RepairHook),{0xD9,0xEE,0xD9,0x5D,0xEC},{255,255,255,255,255},5,0xE8,nullptr},
        {0x7B580C,reinterpret_cast<uintptr_t>(SetupTileHook),{0xE8},{255},5,0xE8,&originalSetup},
        {0x781797,reinterpret_cast<uintptr_t>(EnableButton),{0xE8},{255},5,0xE8,&originalButton},
        {0x7B5D0E,reinterpret_cast<uintptr_t>(AutoCloseHook),{0x8B,0x11,0x8B,0x4D,0xD8},{255,255,255,255,255},5,0xE8,nullptr},
        {0x7B5BF1,reinterpret_cast<uintptr_t>(CanClick),{0xE8},{255},5,0xE8,&originalCanClick},
        {0x7B6B07,reinterpret_cast<uintptr_t>(Populate),{0xE8},{255},5,0xE8,&originalPopulate},
    };
    auto isStewie=[](uintptr_t target) {
        HMODULE owner=nullptr;
        auto stewie=GetModuleHandleA("nvse_stewie_tweaks.dll");
        return stewie && GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(target),&owner) && owner==stewie;
    };
    auto chainFixed=[&](Patch& patch,uint8_t opcode,uintptr_t* fallback) {
        if(Matches(patch)) return true;
        if(*reinterpret_cast<uint8_t*>(patch.address)!=opcode || !isStewie(BranchTarget(patch.address))) {
            snprintf(failure,sizeof(failure),"Unexpected hook at %08X (opcode %02X).",static_cast<unsigned>(patch.address),*reinterpret_cast<uint8_t*>(patch.address));
            return false;
        }
        memcpy(patch.expected,reinterpret_cast<void*>(patch.address),patch.size);
        memset(patch.mask,255,patch.size);
        patch.original=fallback;
        return true;
    };
    if(!chainFixed(patches[0],0xE9,&previousSort) ||
       !chainFixed(patches[3],0xE8,&previousRepair) ||
       !chainFixed(patches[6],0xE8,&previousAutoClose)) return false;
    if(*reinterpret_cast<uint8_t*>(0x781783)!=0xE8 ||
       *reinterpret_cast<uint8_t*>(0x7B6C2C)!=0xA0 || *reinterpret_cast<uint8_t*>(0x7B6C82)!=0x45 ||
       !Matches(patches[2]) || !Matches(patches[4]) ||
       (BranchTarget(0x7B5CD8)!=0x71A5C0 && !isStewie(BranchTarget(0x7B5CD8))) ||
       (BranchTarget(0x7B580C)!=0x4BCDB0 && !isStewie(BranchTarget(0x7B580C)))) {
        snprintf(failure,sizeof(failure),"Conflicting repair-menu hooks; disable other repair-kit menu features and restart."); return false;
    }
    uintptr_t failed=0;
    if(!Apply(patches,failed)) {
        snprintf(failure,sizeof(failure),"Hook validation or page protection failed at %08X.",static_cast<unsigned>(failed)); return false;
    }
    return true;
}
}
