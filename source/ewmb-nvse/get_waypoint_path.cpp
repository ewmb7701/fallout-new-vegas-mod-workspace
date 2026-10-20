#include "get_waypoint_path.h"
#include "nvse/waypoint_path.h"
#include "nvse/GameAPI.h"
#include <cmath>

namespace
{
  NVSEArrayVarInterface *arrays = nullptr;
  bool ExecuteGetWaypointPath(COMMAND_ARGS)
  {
    *result = 0;
    TESObjectREFR *origin = nullptr, *destination = nullptr;
    UInt32 doorAccessMode = 2;
    if (!arrays || !ExtractArgs(EXTRACT_ARGS, &origin, &destination, &doorAccessMode)) return true;
    WaypointPath waypointPath;
    if (!waypointPath.Find(origin, destination, doorAccessMode)) return true;
    const float length = waypointPath.Length();
    if (length < 0) return true;

    using Element = NVSEArrayVarInterface::Element;
    // Preserve the native object's structure in script arrays.
    auto positionMap = [&](const float *position) -> NVSEArrayVarInterface::Array * {
      for (int i = 0; i < 3; ++i) if (!std::isfinite(position[i])) return nullptr;
      const char *keys[] = {"x", "y", "z"};
      Element values[] = {Element::Number(position[0]), Element::Number(position[1]), Element::Number(position[2])};
      return arrays->CreateStringMap(keys, values, 3, scriptObj);
    };
    auto *originPosition = positionMap(waypointPath.origin);
    auto *destinationPosition = positionMap(waypointPath.destination);
    auto *spaces = arrays->CreateArray(nullptr, 0, scriptObj);
    auto *transitions = arrays->CreateArray(nullptr, 0, scriptObj);
    if (!originPosition || !destinationPosition || !spaces || !transitions) return true;
    for (UInt32 i = 0; i < waypointPath.spaces.size; ++i)
    {
      const auto &space = waypointPath.spaces.data[i];
      if (!space.worldspace && !space.cell) return true;
      const char *keys[] = {"exterior", "worldspace", "cell"};
      Element values[] = {Element::Number(space.exterior), Element::Form(space.worldspace), Element::Form(space.cell)};
      auto *entry = arrays->CreateStringMap(keys, values, 3, scriptObj);
      if (!entry) return true;
      arrays->AppendElement(spaces, Element::ArrayValue(entry));
    }
    for (UInt32 i = 0; i < waypointPath.transitions.size; ++i)
    {
      const auto &transition = waypointPath.transitions.data[i];
      if (!transition.door || (transition.door->flags & 0x20)) return true;
      auto *arrival = positionMap(transition.arrival);
      if (!arrival) return true;
      const char *keys[] = {"door", "arrival"};
      Element values[] = {Element::Form(transition.door), Element::ArrayValue(arrival)};
      auto *entry = arrays->CreateStringMap(keys, values, 2, scriptObj);
      if (!entry) return true;
      arrays->AppendElement(transitions, Element::ArrayValue(entry));
    }
    const char *resultKeys[] = {"length", "origin", "destination", "spaces", "transitions"};
    Element values[] = {Element::Number(length), Element::ArrayValue(originPosition),
        Element::ArrayValue(destinationPosition), Element::ArrayValue(spaces), Element::ArrayValue(transitions)};
    auto *waypointPathArray = arrays->CreateStringMap(resultKeys, values, 5, scriptObj);
    if (waypointPathArray) arrays->AssignCommandResult(waypointPathArray, result);
    return true;
  }
  ParamInfo waypointPathParams[] = {{"origin", kParamType_ObjectRef, 0}, {"destination", kParamType_ObjectRef, 0},
      {"doorAccessMode", kParamType_Integer, 1}};
}
void RegisterWaypointPathArrayInterface(NVSEArrayVarInterface *interface) { arrays = interface; }
CommandInfo kCommandInfo_GetWaypointPath = {"GetWaypointPath", "", 0,
    "Returns native spaces and door waypoints with length, or an uninitialized array on failure", 0,
    3, waypointPathParams, ExecuteGetWaypointPath, nullptr, nullptr, 0};
