#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <vector>

// Validate the complete set before changing game code. Call hooks chain the
// existing target, including compatible JIP and UI hooks installed before us.
namespace Hooks {
struct Patch {
    uintptr_t address, target;
    uint8_t expected[8], mask[8]; uint8_t size,opcode;
    uintptr_t* original;
};
inline uintptr_t BranchTarget(uintptr_t site) {
    int32_t displacement; memcpy(&displacement,reinterpret_cast<void*>(site+1),4);
    return site+5+displacement;
}
inline bool Matches(const Patch& p) {
    auto bytes=reinterpret_cast<const uint8_t*>(p.address);
    for(unsigned i=0;i<p.size;++i) if ((bytes[i]&p.mask[i]) != (p.expected[i]&p.mask[i])) return false;
    return true;
}
inline bool Apply(const std::vector<Patch>& patches,uintptr_t& failed) {
    for(auto& p:patches) if(!Matches(p)) { failed=p.address; return false; }
    struct Page { uintptr_t address; DWORD protection; };
    std::vector<Page> pages;
    for(auto& p:patches) {
        auto first=p.address&~uintptr_t(0xFFF),last=(p.address+p.size-1)&~uintptr_t(0xFFF);
        for(auto page=first;page<=last;page+=0x1000) {
            bool seen=false; for(auto& old:pages) if(old.address==page) seen=true;
            if(seen) continue;
            DWORD protection;
            if(!VirtualProtect(reinterpret_cast<void*>(page),0x1000,PAGE_EXECUTE_READWRITE,&protection)) {
                for(auto& old:pages) { DWORD ignored; VirtualProtect(reinterpret_cast<void*>(old.address),0x1000,old.protection,&ignored); }
                failed=p.address; return false;
            }
            pages.push_back({page,protection});
        }
    }
    for(auto& p:patches) {
        if(p.original) *p.original=BranchTarget(p.address);
        auto out=reinterpret_cast<uint8_t*>(p.address);
        out[0]=p.opcode;
        // Jcc has a two-byte opcode, unlike CALL/JMP.
        unsigned prefix=1;
        if(p.opcode==0x0F) { out[1]=0x84; prefix=2; }
        auto relative=static_cast<uint32_t>(p.target-(p.address+prefix+4));
        memcpy(out+prefix,&relative,4);
        memset(out+prefix+4,0x90,p.size-prefix-4);
    }
    for(auto& page:pages) { DWORD ignored; VirtualProtect(reinterpret_cast<void*>(page.address),0x1000,page.protection,&ignored); }
    FlushInstructionCache(GetCurrentProcess(),nullptr,0);
    return true;
}
}
