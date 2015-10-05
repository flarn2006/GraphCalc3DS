#pragma once
#include "TableLayout.h"
#include "Control.h"

template <int _Rows, int _Cols>
class ControlGrid : public TableLayout<Control*, _Rows, _Cols>
{
	TableCell<Control*> *activeCell = nullptr;
	
public:
	ControlGrid()
	{
		this->cellWidth = this->cellHeight = 1;
	}
	
	ControlGrid(int cellWidth, int cellHeight)
	{
		this->cellWidth = cellWidth;
		this->cellHeight = cellHeight;
	}
	
	void Draw()
	{
		auto iter = this->EnumerateCells();
		TableCell<Control*> *cell;
		Rect<int> rect;
		while ((cell = iter.NextCell(rect)) != nullptr) {
			if (cell->content != nullptr) {
				cell->content->Draw(rect.x, rect.y, rect.w, rect.h);
			}
		}
	}
	
	void ScreenTouchStatus(bool touching, int x, int y)
	{
		auto iter = this->EnumerateCells();
		TableCell<Control*> *cell;
		Rect<int> rect;
		while ((cell = iter.NextCell(rect)) != nullptr) {
			if (touching && rect.PointInside(x, y)) {
				cell->content->TouchingInside(rect.x + x, rect.y + y);
			} else if (touching && cell == activeCell) {
				cell->content->TouchingOutside(rect.x + x, rect.y + y);
			} else {
				cell->content->NotTouching();
			}
		}
	}
};