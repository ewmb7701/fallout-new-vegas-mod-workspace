#include "get_exterior_cell_ex.h"
#include "nvse/GameAPI.h"
#include "nvse/exterior_cell.h"

namespace
{
  bool ExecuteGetExteriorCellEx(COMMAND_ARGS)
  {
    *result = 0;
    TESForm *world = nullptr;
    SInt32 x = 0, y = 0;
    if (ExtractArgs(EXTRACT_ARGS, &world, &x, &y))
      if (auto *cell = GetExteriorCellRecord(world, x, y))
        *reinterpret_cast<UInt32 *>(result) = cell->refID;
    return true;
  }
  ParamInfo params[] = {{"worldspace", kParamType_WorldSpace, 0},
      {"cell x", kParamType_Integer, 0}, {"cell y", kParamType_Integer, 0}};
}
CommandInfo kCommandInfo_GetExteriorCellEx = {"GetExteriorCellEx", "", 0,
    "Experimental exterior CELL record lookup without attaching cells or loading children", 0,
    3, params, ExecuteGetExteriorCellEx, nullptr, nullptr, 0};
