#include "ControlGrid.h"

// NOTE:
// This source file only contains code for the base ControlGrid class, which is currently only a single, trivial function.
// If you didn't know that already, you're probably looking for ControlGrid.h, which is where the rest of the code is.
// ControlGrid is mainly a template-based class; the base class only exists to make it possible to switch between different
// screens that have different numbers of rows and columns.

void ControlGridBase::SetDrawOffset(int x, int y)
{
	offsetX = x;
	offsetY = y;
}