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
	sf2d_draw_rectangle(x, y, w, h, RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
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