#pragma once
#include "TableCell.h"
#include "Common.h"

template <typename _Content, int _Rows, int _Cols, typename _CoordType = int>
struct TableLayout
{
	int cellWidth, cellHeight;
	TableCell<_Content> cells[_Rows][_Cols];
	
	TableLayout()
	{
		cellWidth = cellHeight = 1;
	}
	
	TableLayout(int cellWidth, int cellHeight)
	{
		this->cellWidth = cellWidth;
		this->cellHeight = cellHeight;
	}
	
	Rect<_CoordType> CellToRect(int row, int col) const
	{
		return Rect<_CoordType>(cellWidth * col, cellHeight * row, cellWidth * cells[row][col].colSpan, cellHeight * cells[row][col].rowSpan);
	}

	/*const TableCell<_Content> &GetActualCell(int row, int col) const
	{
		int actualRow = row;
		int actualCol = col;

		if (actualRow > 0) {
			for (int r = row - 1; r >= 0; r--) {
				if (r + cells[r][col].rowSpan > row) {
					actualRow = r;
					break;
				}
			}
		}

		if (actualCol > 0) {
			for (int c = col - 1; c >= 0; c--) {
				if (c + cells[row][c].colSpan > col) {
					actualCol = c;
					break;
				}
			}
		}

		return cells[actualRow][actualCol];
	}*/
	
	const TableCell<_Content> *CellAtCoords(int x, int y) const
	{
		RectEnumerator iter = EnumerateCells();
		TableCell<_Content> *cell;
		Rect<int> rect;
		while ((cell = iter.NextCell(rect)) != nullptr) {
			if (rect.PointInside(x, y)) {
				return cell;
			}
		}
		return nullptr;
	}
	
	class RectEnumerator
	{
		struct prevRowInfo_t
		{
			int nextRow;
			int colSpan;
		};

		TableLayout *tbl;
		int row, col;
		prevRowInfo_t prevRowInfo[_Cols];

	public:
		RectEnumerator(TableLayout *tbl)
		{
			this->tbl = tbl;
			row = col = 0;
			for (int i = 0; i < _Cols; i++) {
				prevRowInfo[i] = { 0, 1 };
			}
		}

		TableCell<_Content> *NextCell(Rect<int> &rectOut)
		{
			int resultRow, resultCol;
			bool done = false;
			while (!done) {
				if (row >= _Rows) {
					return nullptr;
				} else {
					resultRow = row;
					resultCol = col;
				}

				if (prevRowInfo[col].nextRow > row) {
					col += prevRowInfo[col].colSpan;
				} else {
					prevRowInfo[col].nextRow = row + tbl->cells[row][col].rowSpan;
					prevRowInfo[col].colSpan = tbl->cells[row][col].colSpan;
					col += tbl->cells[row][col].colSpan;
					done = true;
				}

				if (col >= _Cols) {
					col = 0;
					++row;
				}
			}

			rectOut = tbl->CellToRect(resultRow, resultCol);
			return &tbl->cells[resultRow][resultCol];
		}
	};

	RectEnumerator EnumerateCells()
	{
		return RectEnumerator(this);
	}
};