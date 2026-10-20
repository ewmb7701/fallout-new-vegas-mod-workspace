#include "get_exterior_cell_state.h"
#include "nvse/cell_state.h"
#include "nvse/GameAPI.h"

namespace
{
  NVSEArrayVarInterface *arrays = nullptr;

  bool ExecuteGetExteriorCellState(COMMAND_ARGS)
  {
    *result = 0;
    TESForm *world = nullptr;
    SInt32 x = 0, y = 0;
    CellStateHelpers::CellState state{};
    if (!arrays || !ExtractArgs(EXTRACT_ARGS, &world, &x, &y) ||
        !CellStateHelpers::ReadExteriorCellState(world, x, y, state))
      return true;

    using Element = NVSEArrayVarInterface::Element;
    const char *keys[] = {"visited", "expired"};
    Element values[] = {Element::Number(state.seen ? 1 : 0),
                        Element::Number(CellStateHelpers::Expiry(state))};
    auto *stateArray = arrays->CreateStringMap(keys, values, 2, scriptObj);
    if (stateArray)
      arrays->AssignCommandResult(stateArray, result);
    return true;
  }

  ParamInfo params[] = {{"worldspace", kParamType_WorldSpace, 0},
                        {"cell x", kParamType_Integer, 0},
                        {"cell y", kParamType_Integer, 0}};
}

void RegisterExteriorCellStateArrayInterface(NVSEArrayVarInterface *interface) { arrays = interface; }

CommandInfo kCommandInfo_GetExteriorCellState = {"GetExteriorCellState", "", 0,
                                                 "Returns exterior cell state without loading cells; signed grid coordinates; null on failure", 0,
                                                 3, params, ExecuteGetExteriorCellState, nullptr, nullptr, 0};
