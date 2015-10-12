#pragma once
#include "TableLayout.h"
#include "Control.h"

class ControlGridBase
{
protected:
	int offsetX = 0, offsetY = 0;
	
public:
	void SetDrawOffset(int x, int y);
	virtual void Draw() = 0;
	virtual void ScreenTouchStatus(bool touching, int x, int y) = 0;
};

template <int _Rows, int _Cols>
class ControlGrid : public TableLayout<Control*, _Rows, _Cols>, public ControlGridBase
{
	TableCell<Control*> *activeCell = nullptr;
	bool wasTouchingBefore = false;
	
private:
	void Initialize()
	{
		for (int r=0; r<_Rows; r++) {
			for (int c=0; c<_Cols; c++) {
				this->cells[r][c].content = nullptr;
			}
		}
	}
	
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
	
	void SetDrawOffset(int x, int y)
	{
		offsetX = x;
		offsetY = y;
	}
	
	void Draw()
	{
		auto iter = this->EnumerateCells();
		TableCell<Control*> *cell;
		Rect<int> rect;
		while ((cell = iter.NextCell(rect)) != nullptr) {
			if (cell->content != nullptr) {
				cell->content->Draw(offsetX + rect.x, offsetY + rect.y, rect.w, rect.h);
			}
		}
	}
	
	void ScreenTouchStatus(bool touching, int x, int y)
	{
		x -= offsetX; y -= offsetY;
		auto iter = this->EnumerateCells();
		TableCell<Control*> *cell;
		Rect<int> rect;
		while ((cell = iter.NextCell(rect)) != nullptr) {
			if (cell->content != nullptr) {
				if (touching && rect.PointInside(x, y)) {
					if (!wasTouchingBefore) {
						activeCell = cell;
					}
					if (activeCell == cell || activeCell == nullptr) {
						cell->content->TouchingInside(x - rect.x, y - rect.y);
					}
				} else if (touching && cell == activeCell) {
					cell->content->TouchingOutside(x - rect.x, y - rect.y);
				} else {
					cell->content->NotTouching();
				}
			}
		}
		if (!touching) {
			activeCell = nullptr;
		}
		wasTouchingBefore = touching;
	}
};