#pragma once
#include <3ds.h>
#include <string>
#include <functional>
#include "Control.h"

class TextDisplay : public Control
{
public:
	typedef std::function<void(TextDisplay&)> callback_t;

private:
	std::string text;
	u32 textColor;
	callback_t callback;

protected:
	virtual void Click();
	
public:
	TextDisplay();
	TextDisplay(const std::string &text);

	virtual void Draw(int x, int y, int w, int h);
	
	void SetText(const std::string &text);
	void SetTextColor(u32 color);
	void SetAction(callback_t callback);
};
