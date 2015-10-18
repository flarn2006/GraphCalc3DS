#include <sftd.h>
#include "Button.h"

extern bool altMode;
extern sftd_font *font;

Button::Button()
{
	SetColors(C_BLUE);
}

Button::Button(const std::string &text, ColorPreset colors)
{
	this->text = text;
	SetColors(colors);
}

void Button::Click()
{
	if (callback) {
		callback(*this);
	}
}

void Button::Draw(int x, int y, int w, int h)
{
	u32 c_on = altMode ? color_on_alt : color_on;
	u32 c_off = altMode ? color_off_alt : color_off;
	const char *str = (altMode ? text_alt : text).c_str();
	
	sf2d_draw_rectangle(x+1, y+1, w-2, h-2, GetTouchState() == TS_TOUCHING ? c_on : c_off);
	
	int textX = x + w/2 - sftd_get_text_width(font, 20, str) / 2;
	int textY = y + h/2 - 14;
	sftd_draw_text(font, textX, textY, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), 20, str);
}

void Button::SetText(const std::string &text)
{
	this->text = text_alt = text;
}

void Button::SetColors(u32 off, u32 on)
{
	SetColors(off, on, off, on);
}

void Button::SetColors(u32 off, u32 on, u32 off_alt, u32 on_alt)
{
	SetColors(off, on, false);
	SetColors(off_alt, on_alt, true);
}

void Button::SetColors(u32 off, u32 on, bool alt)
{
	(alt ? color_off_alt : color_off) = off;
	(alt ? color_on_alt : color_on) = on;
}

void Button::SetColors(ColorPreset preset)
{
	SetColors(preset, preset);
}

void Button::SetColors(ColorPreset preset, ColorPreset preset_alt)
{
	SetColors(preset, false);
	SetColors(preset_alt, true);
}

void Button::SetColors(ColorPreset preset, bool alt)
{
	switch (preset) {
		case C_BLUE:
			SetColors(RGBA8(0x00, 0xCC, 0xFF, 0xFF), RGBA8(0xAA, 0xEE, 0xFF, 0xFF), alt);
			break;
		case C_GREEN:
			SetColors(RGBA8(0x5C, 0xE5, 0x00, 0xFF), RGBA8(0xB0, 0xFF, 0x7B, 0xFF), alt);
			break;
		case C_PINK:
			SetColors(RGBA8(0xFF, 0x80, 0xFF, 0xFF), RGBA8(0xFF, 0xCC, 0xFF, 0xFF), alt);
			break;
		case C_ORANGE:
			SetColors(RGBA8(0xFF, 0xCC, 0x00, 0xFF), RGBA8(0xFF, 0xEE, 0xAA, 0xFF), alt);
			break;
	}
}

void Button::SetAction(callback_t callback)
{
	this->callback = callback;
}