#pragma once
#include <cmath>
// Plugin utility, not an engine object or native memory layout.
namespace TravelTime
{
  inline bool ValidDuration(float hours)
  {
    return std::isfinite(hours) && hours >= 0 && hours < 2147483648.0f;
  }
  struct Duration { int whole; float fraction; };
  inline Duration Split(float hours)
  {
    const int whole = static_cast<int>(hours);
    return {whole, hours - whole};
  }
}
