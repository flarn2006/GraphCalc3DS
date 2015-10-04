#pragma once
#include "Common.h"

class ViewWindow
{
	const float sxmin = 0.0f;
	const float sxmax = 399.0f;
	const float symin = 239.0f;
	const float symax = 0.0f;
	
public:
	float xmin, xmax;
	float ymin, ymax;
	
	ViewWindow(float xMin, float xMax, float yMin, float yMax);
	
	Point<int> GetScreenCoords(float x, float y) const;
	Point<int> GetScreenCoords(Point<float> point) const;
	Point<float> GetGraphCoords(int x, int y) const;
	Point<float> GetGraphCoords(Point<int> point) const;
	
	void Pan(float x, float y);
	void ZoomIn(float factor);
	void ZoomOut(float factor);
};