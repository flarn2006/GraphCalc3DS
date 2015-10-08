#include <sf2d.h>
#include <sftd.h>
#include "TextDisplay.h"

extern sftd_font *font;

TextDisplay::TextDisplay()
{
}

TextDisplay::TextDisplay(const std::string &text)
{
	this->text = text;
}

void TextDisplay::Draw(int x, int y, int w, int h)
{
	sf2d_draw_rectangle(x, y, w, h, RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
	sftd_draw_text_wrap(font, x+4, y, textColor, 20, w-4, text.c_str());
}

void TextDisplay::SetText(const std::string &text)
{
	this->text = text;
}

void TextDisplay::SetTextColor(u32 color)
{
	textColor = color;
}