#pragma once

#include <cstdint>

bool CreateLog(const char *path);
void PrintLog(const char *format, ...);

// Write a five-byte x86 relative jump or call.
bool WriteRelJump(std::uintptr_t address, std::uintptr_t target);
bool WriteRelCall(std::uintptr_t address, std::uintptr_t target);

// Decode an x86 E8 rel32 CALL. Caller must verify the opcode and readable bytes.
std::uintptr_t GetRelativeCallTarget(std::uintptr_t address);

// x86 native-call helpers. Addresses and signatures must match the engine ABI.
namespace NativeCall
{
  template <typename R = void, typename... A> R Method(std::uintptr_t address, void *self, A... args)
  { return reinterpret_cast<R(__thiscall *)(void *, A...)>(address)(self, args...); }
  template <typename R = void, typename... A> R Function(std::uintptr_t address, A... args)
  { return reinterpret_cast<R(__cdecl *)(A...)>(address)(args...); }
  template <typename R = void, typename... A> R Virtual(void *self, unsigned offset, A... args)
  { return Method<R>((*reinterpret_cast<std::uintptr_t **>(self))[offset / 4], self, args...); }
}
