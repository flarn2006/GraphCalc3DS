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
	std::string text, text_alt;
	u32 color_off, color_on, color_off_alt, color_on_alt;
	callback_t callback;
	
protected:
	virtual void Click();

public:
	Button();
	Button(const std::string &text, ColorPreset colors = C_BLUE);
	
	virtual void Draw(int x, int y, int w, int h);
	void SetText(const std::string &text);
	void SetText(const std::string &text, const std::string &text_alt);
	void SetText(const std::string &text, bool alt);
	void SetColors(u32 off, u32 on);
	void SetColors(u32 off, u32 on, u32 off_alt, u32 on_alt);
	void SetColors(u32 off, u32 on, bool alt);
	void SetColors(ColorPreset preset);
	void SetColors(ColorPreset preset, ColorPreset preset_alt);
	void SetColors(ColorPreset preset, bool alt);
	void SetAction(callback_t callback);
};