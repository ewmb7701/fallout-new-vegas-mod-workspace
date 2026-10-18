#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

namespace WorldMapBuildHook
{
  inline void MakeJump(UInt8 *bytes, uintptr_t from, uintptr_t to)
  {
    bytes[0] = 0xE9;
    const auto displacement = static_cast<SInt32>(to - from - 5);
    std::memcpy(bytes + 1, &displacement, sizeof(displacement));
  }

  // Relocate the known 1.4.0.525 prologue. Its three complete
  // instructions are position-independent: push ebp; mov ebp,esp; sub esp,174h.
  // Existing entry E9 hooks are chained rather than skipped.
  inline uintptr_t Install(uintptr_t entry, uintptr_t replacement)
  {
    const auto *code = reinterpret_cast<const UInt8 *>(entry);
    constexpr size_t prologueLength = 9;
    UInt8 *trampoline = nullptr;
    uintptr_t original;
    if (code[0] == 0xE9)
    {
      SInt32 displacement;
      std::memcpy(&displacement, code + 1, sizeof(displacement));
      original = entry + 5 + displacement;
    }
    else
    {
      trampoline = static_cast<UInt8 *>(VirtualAlloc(nullptr, prologueLength + 5,
                                                     MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
      std::memcpy(trampoline, code, prologueLength);
      MakeJump(trampoline + prologueLength,
               reinterpret_cast<uintptr_t>(trampoline + prologueLength), entry + prologueLength);
      DWORD old;
      VirtualProtect(trampoline, prologueLength + 5, PAGE_EXECUTE_READ, &old);
      FlushInstructionCache(GetCurrentProcess(), trampoline, prologueLength + 5);
      original = reinterpret_cast<uintptr_t>(trampoline);
    }

    DWORD old;
    VirtualProtect(reinterpret_cast<void *>(entry), 5, PAGE_EXECUTE_READWRITE, &old);
    UInt8 jump[5];
    MakeJump(jump, entry, replacement);
    std::memcpy(reinterpret_cast<void *>(entry), jump, sizeof(jump));
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void *>(entry), sizeof(jump));
    DWORD ignored;
    VirtualProtect(reinterpret_cast<void *>(entry), 5, old, &ignored);
    return original;
  }
}
