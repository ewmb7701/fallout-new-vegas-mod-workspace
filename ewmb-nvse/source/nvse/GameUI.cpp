#include "GameUI.h"
#include "../utility.h"
#include <cmath>

float Tile::GetFloat(std::uint32_t trait)
{
  return reinterpret_cast<float(__thiscall *)(Tile *, std::uint32_t)>(0xA011B0)(this, trait);
}

std::uint32_t Tile::TraitNameToID(const char *name)
{
  return reinterpret_cast<std::uint32_t(__cdecl *)(const char *, std::uint32_t)>(0xA00940)(name, 0xFFFFFFFF);
}

MapMenu *MapMenu::GetSingleton()
{
  return *reinterpret_cast<MapMenu **>(0x11DA368);
}

TESObjectREFR *MapMenu::GetMarkerForTile(Tile *tile) const
{
  if (!tile)
    return nullptr;
  const float value = tile->GetFloat(Tile::TraitNameToID("_MarkerIndex"));
  if (!std::isfinite(value) || value < 0 || value > 4095 || value != std::floor(value))
    return nullptr;
  const MarkerNode *node = &markers;
  for (int i = 0; node && i < static_cast<int>(value); ++i)
    node = node->next;
  return node ? node->marker : nullptr;
}

TESObjectREFR *MapMenu::GetSelectedMarker() const
{
  return GetMarkerForTile(selectedMarkerTile);
}

bool CanPlayerFastTravelWithMessage()
{
  void *player = *reinterpret_cast<void **>(0x11DEA3C);
  constexpr std::uintptr_t eligibilityCall = 0x798026;
  if (!player || *reinterpret_cast<std::uint8_t *>(eligibilityCall) != 0xE8)
    return false;
  // Follow the current map eligibility CALL, including other mods' hooks.
  return reinterpret_cast<bool(__thiscall *)(void *)>(GetRelativeCallTarget(eligibilityCall))(player);
}
