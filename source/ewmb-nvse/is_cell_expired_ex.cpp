#include "is_cell_expired_ex.h"
#include "nvse/cell_state.h"
#include "nvse/GameAPI.h"

namespace
{
  using namespace CellStateHelpers;
  bool ExecuteIsCellExpiredEx(COMMAND_ARGS)
  {
    *result = 0;
    TESForm *cell = nullptr;
    if (!ExtractArgs(EXTRACT_ARGS, &cell) || !cell || cell->typeID != 0x39) return true;
    const auto state = ReadCellState(cell);
    *result = Expiry(state);
    return true;
  }
  ParamInfo stateParams[] = {{"cell", kParamType_AnyForm, 0}};
}
CommandInfo kCommandInfo_IsCellExpiredEx = {"IsCellExpiredEx", "", 0,
    "Checks cell expiry using live or saved detach time", 0,
    1, stateParams, ExecuteIsCellExpiredEx, nullptr, nullptr, 0};
