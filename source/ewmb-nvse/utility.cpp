#include "utility.h"

#include <Windows.h>
#include <cstring>
#include <cstdarg>
#include <cstdio>

FILE *s_log = nullptr;

bool CreateLog(const char *path)
{
  if (s_log)
    fclose(s_log);

  s_log = nullptr;
  return fopen_s(&s_log, path, "w") == 0;
}

void PrintLog(const char *format, ...)
{
  if (!s_log)
    return;

  va_list args;
  va_start(args, format);
  vfprintf(s_log, format, args);
  va_end(args);

  fputc('\n', s_log);
  fflush(s_log);
}

static bool WriteRelativeBranch(std::uintptr_t address, std::uintptr_t target, std::uint8_t opcode)
{
  auto *code = reinterpret_cast<std::uint8_t *>(address);
  DWORD old;
  if (!VirtualProtect(code, 5, PAGE_EXECUTE_READWRITE, &old))
    return false;
  const std::int32_t displacement = static_cast<std::int32_t>(target - address - 5);
  code[0] = opcode;
  std::memcpy(code + 1, &displacement, 4);
  DWORD ignored;
  VirtualProtect(code, 5, old, &ignored);
  FlushInstructionCache(GetCurrentProcess(), code, 5);
  return true;
}

bool WriteRelJump(std::uintptr_t address, std::uintptr_t target)
{
  return WriteRelativeBranch(address, target, 0xE9);
}

bool WriteRelCall(std::uintptr_t address, std::uintptr_t target)
{
  return WriteRelativeBranch(address, target, 0xE8);
}

std::uintptr_t GetRelativeCallTarget(std::uintptr_t address)
{
  return address + 5 + *reinterpret_cast<const std::int32_t *>(address + 1);
}
