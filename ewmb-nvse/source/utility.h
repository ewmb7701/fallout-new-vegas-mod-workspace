#pragma once

#include <cstdint>

bool CreateLog(const char *path);
void PrintLog(const char *format, ...);

// Write a five-byte x86 relative jump or call.
bool WriteRelJump(std::uintptr_t address, std::uintptr_t target);
bool WriteRelCall(std::uintptr_t address, std::uintptr_t target);

// Decode an x86 E8 rel32 CALL. Caller must verify the opcode and readable bytes.
std::uintptr_t GetRelativeCallTarget(std::uintptr_t address);
