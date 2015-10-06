#include <sftd.h>
#include "Button.h"

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
	sf2d_draw_rectangle(x+1, y+1, w-2, h-2, GetTouchState() == TS_TOUCHING ? color_on : color_off);
	
	int textX = x + w/2 - sftd_get_text_width(font, 20, text.c_str()) / 2;
	int textY = y + h/2 - 14;
	sftd_draw_text(font, textX, textY, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), 20, text.c_str());
}

void Button::SetText(const std::string &text)
{
	this->text = text;
}

void Button::SetColors(u32 off, u32 on)
{
	color_off = off;
	color_on = on;
}

void Button::SetColors(ColorPreset preset)
{
	switch (preset) {
		case C_BLUE:
			color_off = RGBA8(0x00, 0xCC, 0xFF, 0xFF);
			color_on = RGBA8(0xAA, 0xEE, 0xFF, 0xFF);
			break;
		case C_GREEN:
			color_off = RGBA8(0x5C, 0xE5, 0x00, 0xFF);
			color_on = RGBA8(0xB0, 0xFF, 0x7B, 0xFF);
			break;
		case C_PINK:
			color_off = RGBA8(0xFF, 0x80, 0xFF, 0xFF);
			color_on = RGBA8(0xFF, 0xCC, 0xFF, 0xFF);
			break;
		case C_ORANGE:
			color_off = RGBA8(0xFF, 0xCC, 0x00, 0xFF);
			color_on = RGBA8(0xFF, 0xEE, 0xAA, 0xFF);
			break;
	}
}

void Button::SetAction(callback_t callback)
{
	this->callback = callback;
}