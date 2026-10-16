#include "fast_travel_ex.h"
#include "nvse/GameAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/travel_time.h"
#include "nvse/cell_coordinates.h"
#include "utility.h"
#include <vector>

namespace
{
  struct Vector { float x, y, z; };
  STATIC_ASSERT(sizeof(Vector) == 12);
  // FalloutNV 1.4.0.525: PlayerCharacter::FastTravel (93CDF0).
  // This is a separate executor: no patches, trampolines, fake marker, or calls
  // into the marker executor. The ordinary coordinate movers own CELL loading.
  template <typename R = void, typename... A> R Method(uintptr_t address, void *self, A... args)
  { return reinterpret_cast<R(__thiscall *)(void *, A...)>(address)(self, args...); }
  template <typename R = void, typename... A> R Function(uintptr_t address, A... args)
  { return reinterpret_cast<R(__cdecl *)(A...)>(address)(args...); }
  template <typename R = void, typename... A> R Virtual(void *self, unsigned offset, A... args)
  { return Method<R>((*reinterpret_cast<uintptr_t **>(self))[offset / 4], self, args...); }
  template <typename T> T &Field(void *self, unsigned offset)
  { return *reinterpret_cast<T *>(static_cast<UInt8 *>(self) + offset); }
  void *const processes = reinterpret_cast<void *>(0x11E0E80);
  void *const clock = reinterpret_cast<void *>(0x11DE7B8);
  void *const actorLock = reinterpret_cast<void *>(0x11F11A0);
  bool running = false;

  bool Finite(Vector v)
  { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }

  void AdvanceClock(float hours)
  {
    const float seconds = hours * 3600.0f / Method<float>(0x867950, clock);
    const float elapsed = Method<float>(0x96D490, processes);
    Method(0x96D4B0, processes, elapsed + seconds);
    Method(0x867A40, clock, seconds);
  }

  void AdvanceTime(void *player, TravelTime::Duration duration, uintptr_t updateLowActors)
  {
    // 93D19D..93D368: native hourly updates, then its sub-hour clock branch.
    // FTP sums already-rounded legs, so retain a fractional remainder even when
    // the total exceeds one hour (native marker travel discards that remainder).
    auto &remaining = Field<int>(player, 0x654);
    remaining = duration.whole;
    Field<UInt8>(player, 0x658) = 0; // travelling, not resting
    while (remaining > 0)
    {
      AdvanceClock(1.0f);
      Method(0x40FBF0, actorLock, 0);
      Method(0x96BCD0, processes, 3600.0f, true);
      Method(0x96DB30, processes, 3600.0f, true, false);
      Method(0x96B810, processes, 3600.0f, true);
      Method(0x96B470, processes, 3600.0f, true);
      Method(updateLowActors, processes, 3600.0f, true);
      Method(0x96EB40, processes);
      Method(0x40FBA0, actorLock);
      --remaining;
      Method(0x88B510, player, 2.0f); // same hourly player recovery as native travel
    }
    if (duration.fraction > 0) AdvanceClock(duration.fraction);
  }

  std::vector<UInt32> Followers(void *player)
  {
    // 973EE0: preserve native eligibility, but retain form IDs across the load.
    // There is no destination reference; move these actors to the player AFTER
    // coordinate relocation, rather than manufacturing a temporary marker.
    std::vector<UInt32> result;
    auto *list = static_cast<UInt8 *>(processes) + 4;
    for (UInt32 i = 0; i < Method<UInt32>(0x55B980, list); ++i)
    {
      auto *actor = Method<TESObjectREFR *>(0x968670, list, i);
      if (!actor || actor == player || !actor->IsActor() || Virtual<bool>(actor, 0x22C, false)) continue;
      if (Method<bool>(0x954A70, player, actor, false) || Method<bool>(0x566950, actor))
        result.push_back(actor->refID);
    }
    return result;
  }

  void MoveFollowers(void *player, const std::vector<UInt32> &followers)
  {
    for (UInt32 id : followers)
    {
      auto *actor = Function<TESObjectREFR *>(0x4839C0, id);
      if (!actor || !actor->IsActor() || (actor->flags & 0x820) || Virtual<bool>(actor, 0x22C, false)) continue;
      Method(0x881680, actor, false);
      Method(0x8AD910, actor, player, false, false);
    }
  }

  bool Travel(TESForm *space, Vector position, Vector rotation, float hours)
{
  auto *player = *reinterpret_cast<void **>(0x11DEA3C);
  auto *main = *reinterpret_cast<void **>(0x11DEA10);
  // CELL bit 0 denotes interiors. Exterior cells are selected by world + XY.
  const bool exterior = space && space->typeID == 0x41;
  const bool interior = space && space->typeID == 0x39 && (Field<UInt8>(space, 0x24) & 1);
  int grid;
  if ((!exterior && !interior) || (space->flags & 0x20) || !Finite(position) || !Finite(rotation) ||
      !TravelTime::ValidDuration(hours) || !player || !main || running || GetPlayerPositionRequest() ||
      !Field<void *>(player, 0x190) || !Method<void *>(0x8D6F30, player) ||
      (exterior && (!CellCoordinates::FromPosition(position.x, grid) || !CellCoordinates::FromPosition(position.y, grid)))) return false;
  auto *actor = Method<void *>(0x9306D0, player);
  if (!actor) return false;
  const float scale = Method<float>(0x867950, clock);
  if (!std::isfinite(scale) || scale <= 0 || !std::isfinite(hours * 3600.0f / scale)) return false;
  // Stewie replaces this helper at its call site. Follow that installed helper
  // (same thiscall ABI) to keep the low-process fixes, without modifying code.
  if (*reinterpret_cast<UInt8 *>(0x93D2C3) != 0xE8) return false;
  const uintptr_t updateLowActors = GetRelativeCallTarget(0x93D2C3);
  const auto duration = TravelTime::Split(hours);
  constexpr float radians = 0.01745329251994329577f;
  rotation = {std::fmod(rotation.x, 360.0f) * radians,
              std::fmod(rotation.y, 360.0f) * radians,
              std::fmod(rotation.z, 360.0f) * radians};
  running = true;

  // 93CE6D..93CEEC: preserve the native travel autosave setting and 20s throttle.
  auto &saveFlags = *reinterpret_cast<UInt32 *>(0x11E0BA4);
  auto &lastSave = *reinterpret_cast<UInt32 *>(0x11E0BA0);
  auto *timer = reinterpret_cast<void *>(0x11F6394);
  if (!(saveFlags & 1)) { saveFlags |= 1; lastSave = Method<UInt32>(0x825C00, timer); }
  const UInt32 elapsed = Method<UInt32>(0x825C00, timer) - lastSave;
  if (*Method<UInt8 *>(0x408D60, reinterpret_cast<void *>(0x11D8980)) && (!elapsed || elapsed > 20000))
  {
    Method(0x850A40, *reinterpret_cast<void **>(0x11DE134));
    lastSave = Method<UInt32>(0x825C00, timer);
  }

  // Shared position-request preamble (93BEBD), followed by 93CEF1..93CF68.
  Method(0x452EB0, main);
  Method(0x66B0D0, static_cast<UInt8 *>(player) + 0xD3C, false);
  Field<UInt8>(player, 0x20C) = 1;
  alignas(4) UInt8 loadingState[12]{};
  Function(0x878160, loadingState, true, true, true);
  Method(0x4539A0, main, true, false);
  Function(0x878250, loadingState[5]);
  Function(0x878200, loadingState);
  Method(0x9EA3B0, Field<void *>(player, 0x190), 0x800);
  Method(0x88B0A0, actor, false);
  Method(0x88D640, player);
  Field<int>(player, 0x654) = duration.whole;
  Field<UInt8>(player, 0x658) = 0;
  // Native loading-screen selection accepts a worldspace or CELL as well as REFR.
  Method(0x457D70, main, true, space, false);
  AdvanceTime(player, duration, updateLowActors);
  const auto followers = Followers(player);

  // 93D37B..93D418: prepare the renderer and perform a complete engine relocation.
  const bool backgroundLoading = Function<bool>(0x93D4F0);
  if (backgroundLoading) Method(0x45AEE0, main, false);
  if (auto *loadingMenu = Function<void *>(0x652110)) Method(0x650A30, loadingMenu, false);
  Function(0xAA7030);
  Method(0x63E8F0, Function<void *>(0x46DD00), true);
  *reinterpret_cast<UInt8 *>(0x11D8906) = 1;
  Method(exterior ? 0x93CCE0 : 0x93C200, player, position, rotation, space, true);
  *reinterpret_cast<UInt8 *>(0x11D8906) = 0;
  MoveFollowers(player, followers);
  Field<int>(player, 0x654) = 0;
  Method(0x63E8F0, Function<void *>(0x46DD00), false);
  if (backgroundLoading) Method(0x45AEE0, main, true);

  // 93D45B..93D4C1: native post-travel process/actor cleanup.
  Method(0x483710, processes);
  Method(0x459870, main);
  Method(0x972D30, processes);
  Method(0x96DF40, processes);
  Field<UInt8>(player, 0x20C) = 0;
  Method(0x975D10, processes);
  Virtual(player, 0x3F4);
  Method(0x961F90, player);
  Field<UInt8>(player, 0x204) = 0;
  Function(0x868D70, true);
  auto *saveManager = *reinterpret_cast<void **>(0x11DDF38);
  if (!Method<bool>(0x42CE10, saveManager) && !Method<bool>(0x93C1E0, player))
    Method(0x5D14D0, player, true);
  running = false;
  return true;
}

  bool ExecuteFastTravelEx(COMMAND_ARGS)
  {
    *result = 0;
    TESForm *space = nullptr;
    Vector position{}, rotation{};
    float hours = 0;
    if (ExtractArgs(EXTRACT_ARGS, &space, &position.x, &position.y, &position.z,
                    &rotation.x, &rotation.y, &rotation.z, &hours))
      *result = Travel(space, position, rotation, hours) ? 1 : 0;
    return true;
  }
  ParamInfo params[] = {
      {"worldspace or interior cell", kParamType_AnyForm, 0},
      {"x", kParamType_Float, 0}, {"y", kParamType_Float, 0}, {"z", kParamType_Float, 0},
      {"rotation x (degrees)", kParamType_Float, 0}, {"rotation y (degrees)", kParamType_Float, 0},
      {"rotation z (degrees)", kParamType_Float, 0}, {"game hours", kParamType_Float, 0}};
}
CommandInfo kCommandInfo_FastTravelEx = {"FastTravelEx", "", 0,
    "Travels to coordinates in a worldspace or interior cell with explicit elapsed game hours", 0,
    8, params, ExecuteFastTravelEx, nullptr, nullptr, 0};
