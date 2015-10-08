#pragma once
#include <3ds.h>
#include <string>
#include "Control.h"

class TextDisplay : public Control
{
	std::string text;
	u32 textColor;
	
public:
	TextDisplay();
	TextDisplay(const std::string &text);

	virtual void Draw(int x, int y, int w, int h);
	
	void SetText(const std::string &text);
	void SetTextColor(u32 color);
};