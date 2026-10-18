#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cmath>
namespace SavedCellState
{
  inline bool Visited(bool liveSeen, std::uint32_t savedFlags)
  {
    return liveSeen || (savedFlags & 0x80000000) != 0;
  }
  // Match JohnnyGuitar IsCellExpired: missing/zero time is -1; resets are 1.
  inline int Expired(bool hasTime, std::int32_t time, float days, std::uint32_t resetHours)
  {
    if (!hasTime || !time) return -1;
    if (time == -1 || time == -2) return 1;
    const float elapsed = static_cast<float>(std::floor(static_cast<double>(days) * 24)) - static_cast<float>(time);
    return elapsed >= static_cast<float>(resetHours) ? 1 : 0;
  }
  // In-memory BGSFormChange data: packed type/length width, version, length,
  // then CELL preload data. Verified against 84E730, 84E340 and 84E3C0.
  inline std::size_t HeaderSize(std::uint8_t type)
  {
    return (type & 63) == 7 && (type >> 6) < 3 ? 2 + (std::size_t(1) << (type >> 6)) : 0;
  }
  struct Exterior { std::uint16_t worldIndex; int x, y; std::int32_t detachTime; };
  inline bool ExteriorPosition(std::uint32_t flags, const std::uint8_t *data, std::size_t size, Exterior &out)
  {
    if (!(flags & 0x40000000) || !(flags & 0x30000000) || !data || !size) return false;
    const auto header = HeaderSize(data[0]);
    if (!header || size < header) return false;
    std::uint32_t length = 0;
    std::memcpy(&length, data + 2, header - 2);
    const std::size_t offset = flags & 0x20000000 ? 4 : 6;
    if (length < offset + 4 || size < header + offset + 4) return false;
    const auto *prefix = data + header;
    std::memcpy(&out.worldIndex, prefix, 2);
    if (offset == 4)
    {
      out.x = static_cast<std::int8_t>(prefix[2]);
      out.y = static_cast<std::int8_t>(prefix[3]);
    }
    else
    {
      std::int16_t x, y;
      std::memcpy(&x, prefix + 2, 2); std::memcpy(&y, prefix + 4, 2);
      out.x = x; out.y = y;
    }
    std::memcpy(&out.detachTime, prefix + offset, 4);
    return true;
  }
  inline bool DetachTime(std::uint32_t flags, const std::uint8_t *data, std::size_t size, std::int32_t &time)
  {
    if (!(flags & 0x40000000) || !data || !size) return false;
    const auto header = HeaderSize(data[0]);
    if (!header || size < header) return false;
    std::uint32_t length = 0;
    std::memcpy(&length, data + 2, header - 2);
    const std::size_t offset = flags & 0x20000000 ? 4 : flags & 0x10000000 ? 6 : 0;
    if (length < offset + 4 || size < header + offset + 4) return false;
    std::memcpy(&time, data + header + offset, 4);
    return true;
  }
}
