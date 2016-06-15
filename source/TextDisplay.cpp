#include <sf2d.h>
#include "BmpFont.h"
#include "TextDisplay.h"

extern BmpFont mainFont;

TextDisplay::TextDisplay()
{
}

TextDisplay::TextDisplay(const std::string &text)
{
	this->text = text;
}

void TextDisplay::Draw(int x, int y, int w, int h)
{
	sf2d_draw_rectangle_gradient(x, y, w, h, RGBA8(0xD0, 0xD0, 0xD0, 0xFF), RGBA8(0xFF, 0xFF, 0xFF, 0xFF), SF2D_TOP_TO_BOTTOM);
    mainFont.drawStrWrap(text, x+4, y, w-4, textColor);
}

void TextDisplay::SetText(const std::string &text)
{
	this->text = text;
}

void TextDisplay::SetTextColor(u32 color)
{
	textColor = color;
}