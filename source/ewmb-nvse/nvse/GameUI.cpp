#include "GameUI.h"
#include "../utility.h"
#include <cmath>

TESObjectREFR *GetEnemyHealthTarget()
{
  auto *hud = *reinterpret_cast<std::uint8_t **>(0x11D96C0);
  return hud ? *reinterpret_cast<TESObjectREFR **>(hud + 0x21C) : nullptr;
}

bool SetEnemyHealthTarget(TESObjectREFR *target)
{
  auto *hud = *reinterpret_cast<std::uint8_t **>(0x11D96C0);
  if (!hud)
    return false;
  *reinterpret_cast<TESObjectREFR **>(hud + 0x21C) = target;
  if (target)
  {
    // Native setter tail (773251..7732A9): refresh the name and display timer.
    const char *name = reinterpret_cast<const char *(__thiscall *)(TESObjectREFR *)>(0x55D520)(target);
    auto *nameTile = *reinterpret_cast<Tile **>(hud + 0x84);
    reinterpret_cast<void(__thiscall *)(Tile *, std::uint32_t, const char *, bool)>(0xA01350)(
        nameTile, 0xFC4, name, true);
    reinterpret_cast<void(__thiscall *)(void *)>(0x771520)(hud + 0x224);
    *reinterpret_cast<std::uint32_t *>(hud + 0x244) =
        reinterpret_cast<std::uint32_t(__thiscall *)(void *)>(0x825C00)(hud + 0x224);
  }

  return true;
}

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

bool GetPCCanFastTravelWithMessage()
{
  void *player = *reinterpret_cast<void **>(0x11DEA3C);
  constexpr std::uintptr_t eligibilityCall = 0x798026;
  if (!player || *reinterpret_cast<std::uint8_t *>(eligibilityCall) != 0xE8)
    return false;
  // Follow the current map eligibility CALL, including other mods' hooks.
  return reinterpret_cast<bool(__thiscall *)(void *)>(GetRelativeCallTarget(eligibilityCall))(player);
}
