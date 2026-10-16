#include "get_path.h"
#include "nvse/travel_path.h"
#include "nvse/GameAPI.h"
#include <cmath>

namespace
{
  NVSEArrayVarInterface *arrays = nullptr;
  bool ExecuteGetPath(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *origin = nullptr, *destination = nullptr;
    if (!arrays || !ExtractArgs(EXTRACT_ARGS, &origin, &destination)) return true;
    TravelPath path;
    if (!path.Find(origin, destination)) return true;
    const float distance = path.Distance();
    if (distance < 0) return true;

    using Element = NVSEArrayVarInterface::Element;
    auto *points = arrays->CreateArray(nullptr, 0, scriptObj);
    if (!points) return true;
    const char *keys[] = {"reference", "worldspace", "cell", "x", "y", "z", "kind"};
    // Convert directly from native storage while path owns it. No Point/Result copy.
    auto append = [&](TESObjectREFR *ref, const TravelPath::Space &space,
                      const float *position, const char *kind) {
      if (!ref || (ref->flags & 0x20) || (!space.worldspace && !space.cell) || !position) return false;
      for (int i = 0; i < 3; ++i) if (!std::isfinite(position[i])) return false;
      Element values[] = {Element::Form(ref), Element::Form(space.worldspace), Element::Form(space.cell),
          Element::Number(position[0]), Element::Number(position[1]), Element::Number(position[2]), Element::String(kind)};
      auto *entry = arrays->CreateStringMap(keys, values, 7, scriptObj);
      if (!entry) return false;
      arrays->AppendElement(points, Element::ArrayValue(entry));
      return true;
    };
    if (!append(origin, path.spaces.data[0], path.origin, "origin")) return true;
    for (UInt32 i = 0; i < path.transitions.size; ++i)
    {
      const auto &transition = path.transitions.data[i];
      const auto *teleport = TravelPath::GetTeleport(transition.door);
      if (!teleport ||
          !append(transition.door, path.spaces.data[i], transition.door->GetPosition(), "exit") ||
          !append(teleport->linkedDoor, path.spaces.data[i + 1], transition.arrival, "arrival")) return true;
    }
    if (!append(destination, path.spaces.data[path.spaces.size - 1], path.destination, "destination")) return true;
    const char *resultKeys[] = {"distance", "points"};
    Element values[] = {Element::Number(distance), Element::ArrayValue(points)};
    auto *pathArray = arrays->CreateStringMap(resultKeys, values, 2, scriptObj);
    if (pathArray) arrays->AssignCommandResult(pathArray, result);
    return true;
  }
  ParamInfo params[] = {{"origin", kParamType_ObjectRef, 0}, {"destination", kParamType_ObjectRef, 0}};
}
void RegisterPathArrayInterface(NVSEArrayVarInterface *interface) { arrays = interface; }
CommandInfo kCommandInfo_GetPath = {"GetPath", "", 0,
    "Returns native spaces and door waypoints with distance, or an uninitialized array on failure", 0,
    2, params, ExecuteGetPath, nullptr, nullptr, 0};
