#include "exterior_cell.h"
#include "../utility.h"

TESForm *GetExteriorCellRecord(TESForm *world, int x, int y)
{
  if (!world || world->typeID != 0x41 || (world->flags & 0x20) ||
      x < -32768 || x > 32767 || y < -32768 || y > 32767)
    return nullptr;
  if (auto *cell = NativeCall::Method<TESForm *>(0x5875A0, world, x, y))
    return cell;

  auto *loader = *reinterpret_cast<void **>(0x11DE45C);
  auto *save = *reinterpret_cast<void **>(0x11DDF38);
  if (!loader || !save || !*reinterpret_cast<void **>(0x11C3F2C))
    return nullptr;
  // Do not interfere with save/load or a cell's child-record loading context.
  const auto flags = *reinterpret_cast<UInt32 *>(static_cast<UInt8 *>(save) + 0x244);
  if ((flags & 6) || *reinterpret_cast<void **>(0x11C3F30))
    return nullptr;

  TESForm *cell = nullptr;
  bool loaded = true;
  // 585BAE..585D36: visit contributing files in native order and locate the CELL.
  const UInt32 count = NativeCall::Method<UInt32>(0x5504E0, world);
  for (UInt32 i = 0; i < count; ++i)
  {
    auto *file = NativeCall::Method<void *>(0x484E60, world, i);
    if (!file)
      continue;
    file = NativeCall::Method<void *>(0x4739B0, file); // Thread's file reader.
    if (!file || !NativeCall::Method<bool>(0x5854F0, world, file, x, y))
      continue;
    if (!cell)
    {
      void *memory = NativeCall::Function<void *>(0x401000, 0xE0);
      if (!memory)
        return nullptr;
      cell = NativeCall::Method<TESForm *>(0x5415B0, memory);
      NativeCall::Method(0x544300, cell, false); // Exterior.
      NativeCall::Method(0x544630, cell);        // Allocate exterior coordinate data.
      NativeCall::Method(0x544C90, cell, x, y);
      // Register with the explicit world; 587670 sets CELL::worldSpace via
      // 54DE10 before Load/InitItem. The record dispatcher uses 11C3F34, but
      // this record-only sequence bypasses it and must not overwrite its context.
      NativeCall::Method(0x587670, world, cell);
      const UInt32 savedID = NativeCall::Method<UInt32>(0x861640, loader, world->refID, x, y);
      if (savedID)
        NativeCall::Virtual(cell, 0x128, savedID, true);
      loaded = NativeCall::Function<bool>(0x4601D0, cell, file);
    }
    else
    {
      auto *header = NativeCall::Method<UInt8 *>(0x462270, file);
      if (!(*reinterpret_cast<UInt32 *>(header + 8) & 0x4000))
        loaded = NativeCall::Function<bool>(0x4601D0, cell, file) && loaded;
    }
    // Deliberately omit 550500: it reads child REFR/ACHR/ACRE records and leaves
    // their runtime fixup to the normal loading pipeline. A state query needs
    // only CELL data. Do not set the children-loaded bit via 551440 either.
  }
  if (!cell)
    return nullptr;
  // 585D4B..585D8C: resolve the CELL's own form references, not child references.
  const bool mode = NativeCall::Method<bool>(0x47C850, loader);
  NativeCall::Method(0x4534F0, loader, !mode);
  NativeCall::Virtual(cell, 0x88); // TESObjectCELL::InitItem.
  NativeCall::Method(0x4534F0, loader, mode);
  // Unlike 585B30, omit 588150 (redistributes existing worldspace references).
  // The un-set children-loaded bit lets ordinary cell loading do that later.
  const bool valid = loaded && cell->typeID == 0x39 && cell->refID && !(cell->flags & 0x20);
  return valid ? cell : nullptr;
}
