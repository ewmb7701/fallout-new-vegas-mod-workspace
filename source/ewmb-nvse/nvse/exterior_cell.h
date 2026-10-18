#pragma once
#include "GameForms.h"

// Experimental record-only lookup. Coordinates are signed 16-bit cell indices.
// The returned CELL is engine-owned; this does not attach it or load its children.
TESForm *GetExteriorCellRecord(TESForm *world, int x, int y);
