#pragma once
#include <sf2d.h>
#include <functional>
#include <string>
#include "Control.h"

class Button : public Control
{
public:
	typedef std::function<void(Button&)> callback_t;
	enum ColorPreset { C_BLUE, C_GREEN, C_PINK, C_ORANGE };
	
private:
	std::string text;
	u32 color_off, color_on;
	callback_t callback;
	
protected:
	virtual void Click();

public:
	Button();
	Button(const std::string &text, ColorPreset colors = C_BLUE);
	
	virtual void Draw(int x, int y, int w, int h);
	void SetText(const std::string &text);
	void SetColors(u32 off, u32 on);
	void SetColors(ColorPreset preset);
	void SetAction(callback_t callback);
};