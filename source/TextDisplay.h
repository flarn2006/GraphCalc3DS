#pragma once
#include <string>
#include "Control.h"

class TextDisplay : public Control
{
	std::string text;
	
public:
	TextDisplay();
	TextDisplay(const std::string &text);

	virtual void Draw(int x, int y, int w, int h);
	
	void SetText(const std::string &text);
};