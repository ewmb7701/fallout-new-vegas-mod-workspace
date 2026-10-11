#pragma once

#include <cstddef>
#include <cstdint>

class TESObjectREFR;

bool CanPlayerFastTravelWithMessage();

// Native UI API for FalloutNV 1.4.0.525. Tile layout is opaque here.
class Tile
{
public:
  float GetFloat(std::uint32_t trait);
  static std::uint32_t TraitNameToID(const char *name);
};

// Partial runtime layout: only the fields used by this plugin are declared.
// This is a view of engine-owned memory; do not allocate MapMenu instances.
class MapMenu
{
public:
  struct MarkerNode
  {
    TESObjectREFR *marker;
    MarkerNode *next;
  };

  std::uint8_t opaque000[0x80];
  std::uint32_t currentTab;                 // 080
  std::uint8_t opaque084[0xD4 - 0x84];
  Tile *selectedMarkerTile;                 // 0D4
  MarkerNode markers;                      // 0D8

  static MapMenu *GetSingleton();
  bool IsWorldMap() const { return currentTab == 0x21; }
  TESObjectREFR *GetMarkerForTile(Tile *tile) const;
  TESObjectREFR *GetSelectedMarker() const;
};

static_assert(offsetof(MapMenu, currentTab) == 0x80);
static_assert(offsetof(MapMenu, selectedMarkerTile) == 0xD4);
static_assert(offsetof(MapMenu, markers) == 0xD8);

