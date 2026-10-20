#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <algorithm>

// Narrow ABI views for FalloutNV 1.4.0.525. Engine objects remain engine-owned.
namespace Game {
template<class R = void, class... A> R This(uintptr_t address, void* self, A... args) {
    return reinterpret_cast<R(__thiscall*)(void*, A...)>(address)(self, args...);
}
template<class R = void, class... A> R Call(uintptr_t address, A... args) {
    return reinterpret_cast<R(__cdecl*)(A...)>(address)(args...);
}
inline void* Allocate(size_t size) { return This<void*>(0xAA3E40, reinterpret_cast<void*>(0x11F6238), size); }
inline void Free(void* p) { This(0xAA4060, reinterpret_cast<void*>(0x11F6238), p); }
struct Form { uint32_t vtable; uint8_t type; uint8_t pad[3]; uint32_t flags, id; };
template<class T> struct Node {
    T* data; Node* next;
    void Append(T* item) {
        auto node = this;
        while (node->next) node = node->next;
        if (!node->data) node->data = item;
        else { auto added = static_cast<Node*>(Allocate(sizeof(Node))); *added = {item,nullptr}; node->next = added; }
    }
    void Insert(T* item) {
        if (!data) { data = item; return; }
        auto added = static_cast<Node*>(Allocate(sizeof(Node))); *added = *this;
        data = item; next = added;
    }
    void Clear() {
        auto node = next; data = nullptr; next = nullptr;
        while (node) { auto following = node->next; Free(node); node = following; }
    }
};
struct Entry {
    Node<void>* extra; int count; Form* form;
    float Health() { return This<float>(0x4BCDB0,this,true); }
    static Entry* Create(Form* form, int count) {
        auto entry = static_cast<Entry*>(Allocate(sizeof(Entry)));
        auto extra = static_cast<Node<void>*>(Allocate(sizeof(Node<void>)));
        *extra = {}; *entry = {extra,count,form}; return entry;
    }
};
struct Tile {
    float Get(uint32_t id) { return This<float>(0xA011B0,this,id); }
    void Set(uint32_t id,float value) { This(0xA012D0,this,id,value,true); }
    void Text(const char* text) { This(0xA01350,this,0xFC4u,text,true); }
    Tile* Child(int id) { return This<Tile*>(0xA03EB0,this,id); }
};
struct ListItem { Tile* tile; Entry* entry; uint8_t filtered; uint8_t pad[3]; };
struct ItemList {
    uintptr_t* vtable; Node<ListItem> items;
    Tile *parent,*selected,*scroll;
    const char* templateName;
    uint16_t count,pad;
    float totalHeight,storedIndex,storedScroll;
    uint16_t flags,pad2;
    Entry* Selected() {
        for (auto node=&items; node; node=node->next)
            if (node->data && node->data->tile == selected) return node->data->entry;
        return nullptr;
    }
    Tile* Insert(Entry* entry,const char* text) {
        if (!parent || !templateName) return nullptr;
        auto menu=This<void*>(0xA03C90,parent);
        auto tile=This<Tile*>(0xA1DDB0,menu,parent,templateName,static_cast<void*>(nullptr));
        if (!tile) return nullptr;
        tile->Set(0xFAA,14); tile->Text(text);
        auto item=static_cast<ListItem*>(Allocate(sizeof(ListItem)));
        *item={tile,entry,0,{}}; items.Append(item);
        if (flags & 1) { This(0x7269D0,this,tile); This(0x71AD30,this); }
        tile->Set(0xFAC,static_cast<float>(count++));
        if (count == 1) {
            auto trait=Call<uint32_t>(0xA00940,"_number_of_visible_items",-1);
            float visible=parent->Get(trait);
            if (visible > 0) {
                auto value=This<void*>(0xA00E90,parent,0xFB0u);
                This(0xA09200,value);
                This(0xA09130,value,0x7D0u,tile,0xFB0u);
                This(0xA09080,value,0x7D3u,visible);
                This(0xA09410,value,false);
            }
        }
        return tile;
    }
    void Remove(Tile* tile) { This(0x71A5C0,this,tile); }
    void RemoveForm(Form* form) { This(0x71A200,this,form); }
    void SaveScroll() { This(0x7312E0,this); }
    void RestoreScroll() { This(0x731360,this,false); }
    void Sort() { This(vtable[8],this,*reinterpret_cast<uintptr_t*>(0x7B7310)); }
};
struct RepairMenu {
    uint8_t opaque[0x38]; Tile* brokenHealth;
    uint8_t opaque2[0x20]; ItemList items;
};
static_assert(sizeof(Entry)==12 && sizeof(ItemList)==0x30);
static_assert(offsetof(RepairMenu,items)==0x5C && offsetof(Form,id)==12);
inline Entry** repairTarget=reinterpret_cast<Entry**>(0x11DA760);
inline Entry** inventoryTarget=reinterpret_cast<Entry**>(0x11D9EA8);
inline RepairMenu* Menu() { return *reinterpret_cast<RepairMenu**>(0x11DA75C); }
inline void* Player() { return *reinterpret_cast<void**>(0x11DEA3C); }
inline Form* Lookup(uint32_t id) { return Call<Form*>(0x4839C0,id); }
inline int Count(Form* form) {
    if (!form || !Player()) return 0;
    auto extra=This<uint8_t*>(0x410220,static_cast<uint8_t*>(Player())+0x44,0x15u);
    auto changes=extra ? *reinterpret_cast<void**>(extra+12) : nullptr;
    return changes ? This<int>(0x4C8F30,changes,form) : 0;
}
inline float RepairSkill() { return This<float>(0x66EF50,static_cast<uint8_t*>(Player())+0xA4,39u); }
inline const char* KitName(Form* kit) {
    // Both supported kits are ALCH: MagicItem/TESFullName at +30, string at +34.
    auto name=*reinterpret_cast<const char**>(reinterpret_cast<uint8_t*>(kit)+0x34);
    return name ? name : "Repair Kit";
}
}
