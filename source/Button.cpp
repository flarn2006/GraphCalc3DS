#include <sf2d.h>
#include "Button.h"

void Button::Click()
{
	if (callback) {
		callback(*this);
	}
}

void Button::Draw(int x, int y, int w, int h)
{
	sf2d_draw_rectangle(x+1, y+1, w-2, h-2, GetTouchState() == TS_TOUCHING ? RGBA8(0xAA, 0xEE, 0xFF, 0xFF) : RGBA8(0x00, 0xCC, 0xFF, 0xFF));
}

void Button::SetAction(callback_t callback)
{
	this->callback = callback;
}