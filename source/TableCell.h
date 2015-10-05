#pragma once

template <typename _Content>
struct TableCell
{
	_Content content;
	int rowSpan;
	int colSpan;
	
	TableCell()
	{
		content = _Content();
		rowSpan = colSpan = 1;
	}
	
	TableCell(_Content content)
	{
		this->content = content;
		rowSpan = colSpan = 1;
	}
	
	TableCell(_Content content, int rowSpan, int colSpan)
	{
		this->content = content;
		this->rowSpan = rowSpan;
		this->colSpan = colSpan;
	}
};