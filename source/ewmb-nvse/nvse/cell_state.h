#pragma once
#include "GameObjects.h"
#include <cstdint>

// Shared implementation details for the cell-state commands.
namespace CellStateHelpers
{
  struct Change { UInt32 flags; const UInt8 *data; };
  struct ChangeNode { ChangeNode *next; UInt32 key; Change *value; };
  struct ChangeMap { void *vtable; UInt32 buckets; ChangeNode **table; UInt32 count; };
  struct WorldIndexNode { WorldIndexNode *next; UInt32 key; UInt32 value; };
  struct WorldIndexMap { void *vtable; UInt32 buckets; WorldIndexNode **table; UInt32 count; };
  struct CellState { bool seen; int detachStatus; std::int32_t detachTime; };
  CellState ReadCellState(TESForm *cell);
  CellState ReadSavedExteriorCellState(const ChangeMap *changes, const WorldIndexMap *worlds,
      UInt32 worldID, int x, int y);
  // Signed grid coordinates, not world positions. Never loads or registers CELLs.
  bool ReadExteriorCellState(TESForm *world, int x, int y, CellState &state);
  int Expiry(const CellState &state);
}
