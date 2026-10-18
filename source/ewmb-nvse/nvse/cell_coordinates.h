#pragma once
#include <cmath>
namespace CellCoordinates
{
  inline bool FromPosition(float position, int &coordinate)
  {
    if (!std::isfinite(position)) return false;
    const double grid = std::floor(static_cast<double>(position) / 4096.0);
    if (grid < -32768 || grid > 32767) return false;
    coordinate = static_cast<int>(grid);
    return true;
  }
}
