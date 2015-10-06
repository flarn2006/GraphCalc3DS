#include "Control.h"

Control::Control()
{
	touchState = TS_INACTIVE;
}

Control::~Control()
{
}

void Control::Draw(int x, int y, int w, int h)
{
}

void Control::TouchStart(int x, int y)
{
}

void Control::TouchEnd()
{
}

void Control::Click()
{
}

void Control::TouchingInside(int x, int y)
{
	TouchState oldState = touchState;
	touchState = TS_TOUCHING;
	if (oldState == TS_INACTIVE) {
		TouchStart(x, y);
	}
}

void Control::TouchingOutside(int x, int y)
{
	touchState = TS_ACTIVE;
}

void Control::NotTouching()
{
	TouchState oldState = touchState;
	touchState = TS_INACTIVE;
	if (oldState != TS_INACTIVE) {
		TouchEnd();
		if (oldState == TS_TOUCHING) {
			Click();
		}
	}
}

Control::TouchState Control::GetTouchState() const
{
	return touchState;
}