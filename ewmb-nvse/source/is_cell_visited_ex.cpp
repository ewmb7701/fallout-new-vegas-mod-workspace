#include "is_cell_visited_ex.h"
#include "nvse/cell_state.h"
#include "nvse/GameAPI.h"

namespace
{
  using namespace CellStateHelpers;
  bool ExecuteIsCellVisitedEx(COMMAND_ARGS)
  {
    *result = 0;
    TESForm *cell = nullptr;
    if (ExtractArgs(EXTRACT_ARGS, &cell) && cell && cell->typeID == 0x39)
      *result = ReadCellState(cell).seen ? 1 : 0;
    return true;
  }
  ParamInfo stateParams[] = {{"cell", kParamType_AnyForm, 0}};
}
CommandInfo kCommandInfo_IsCellVisitedEx = {"IsCellVisitedEx", "", 0,
    "Checks live or saved seen data; detach data alone does not count as visited", 0,
    1, stateParams, ExecuteIsCellVisitedEx, nullptr, nullptr, 0};
