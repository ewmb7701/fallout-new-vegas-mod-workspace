#include "cell_state.h"
#include "saved_cell_state.h"
#include <cstring>

namespace CellStateHelpers
{
  Change *FindCellChange(UInt32 id)
  {
    auto *manager = *reinterpret_cast<void **>(0x11DDF38);
    if (!manager) return nullptr;
    auto *map = *reinterpret_cast<ChangeMap **>(manager);
    if (!map || !map->table || !map->buckets) return nullptr;
    for (auto *entry = map->table[id % map->buckets]; entry; entry = entry->next)
      if (entry->key == id) return entry->value;
    return nullptr;
  }
  CellState ReadCellState(TESForm *cell)
  {
    // Live extras are authoritative. When seen data is absent,
    // consult the save-change map without applying changes or attaching the cell.
    auto getExtra = reinterpret_cast<void *(__thiscall *)(void *, UInt32)>(0x410220);
    auto *extras = reinterpret_cast<UInt8 *>(cell) + 0x28;
    auto *seen = static_cast<UInt8 *>(getExtra(extras, 5));
    auto *detached = static_cast<UInt8 *>(getExtra(extras, 0xB));
    bool hasSeenData = seen && *reinterpret_cast<void **>(seen + 0xC);
    std::int32_t detachTime = 0;
    int detachStatus = detached ? 1 : 0; // 0 absent, 1 decoded, -1 unreadable.
    if (detached) std::memcpy(&detachTime, detached + 0xC, 4);
    if (!hasSeenData)
    {
      if (auto *change = FindCellChange(cell->refID))
      {
        hasSeenData = SavedCellState::Visited(hasSeenData, change->flags);
        if (!detached && (change->flags & 0x40000000))
        {
          detachStatus = -1; // Preserve the distinction between absent and unreadable.
          if (change->data)
          {
            const auto header = SavedCellState::HeaderSize(change->data[0]);
            const auto offset = change->flags & 0x20000000 ? 4 : change->flags & 0x10000000 ? 6 : 0;
            // Only read the fixed preload prefix; never parse the variable seen data.
            if (SavedCellState::DetachTime(change->flags, change->data, header ? header + offset + 4 : 1, detachTime))
              detachStatus = 1;
          }
        }
      }
    }
    return {hasSeenData, detachStatus, detachTime};
  }
  int Expiry(const CellState &state)
  {
    // 1.4.0.525: GameDaysPassed global (form 39), and iHoursToRespawnCell.
    // LookupFormByID; 0x483A00 instead expects an editor-ID string.
    auto lookup = reinterpret_cast<TESForm *(__cdecl *)(UInt32)>(0x4839C0);
    auto *days = lookup(0x39);
    const float gameDays = days ? *reinterpret_cast<float *>(reinterpret_cast<UInt8 *>(days) + 0x24) : 1.0f;
    return SavedCellState::Expired(state.detachStatus == 1, state.detachTime,
        gameDays, *reinterpret_cast<UInt32 *>(0x11CA164));
  }
}
