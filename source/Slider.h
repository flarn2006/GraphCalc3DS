#pragma once
#include "Control.h"

class Slider : public Control
{
	double min, max;
	int width;
	
	void TouchingAnywhere(int x, int y);
	
public:
	double value;
	
	Slider();
	Slider(double min, double max);
	
	virtual void Draw(int x, int y, int w, int h);
	virtual void TouchingInside(int x, int y);
	virtual void TouchingOutside(int x, int y);
	
	void SetRange(double min, double max);
	void SetMinimum(double min);
	void SetMaximum(double max);
	double GetMinimum() const;
	double GetMaximum() const;
};