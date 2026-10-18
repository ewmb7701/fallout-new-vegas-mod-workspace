#pragma once
#include "GameObjects.h"
#include <cstdint>

// Shared implementation details for the cell-state commands.
namespace CellStateHelpers
{
  struct Change { UInt32 flags; const UInt8 *data; };
  struct ChangeNode { ChangeNode *next; UInt32 key; Change *value; };
  struct ChangeMap { void *vtable; UInt32 buckets; ChangeNode **table; UInt32 count; };
  struct CellState { bool seen; int detachStatus; std::int32_t detachTime; };
  CellState ReadCellState(TESForm *cell);
  int Expiry(const CellState &state);
}
