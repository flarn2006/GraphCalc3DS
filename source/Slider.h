#pragma once
#include "Control.h"

class Slider : public Control
{
	float min, max;
	int width;
	
	void TouchingAnywhere(int x, int y);
	
public:
	float value;
	
	Slider();
	Slider(float min, float max);
	
	virtual void Draw(int x, int y, int w, int h);
	virtual void TouchingInside(int x, int y);
	virtual void TouchingOutside(int x, int y);
	
	void SetRange(float min, float max);
	float GetMinimum() const;
	float GetMaximum() const;
};