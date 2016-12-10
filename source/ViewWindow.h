#pragma once
#include "Common.h"

class ViewWindow
{
	static constexpr double sxmin = 0.0;
	static constexpr double sxmax = 399.0;
	static constexpr double symin = 239.0;
	static constexpr double symax = 0.0;
	
public:
	double xmin, xmax;
	double ymin, ymax;
	double offset;
	bool offsetInvalid;
	
	ViewWindow(double xMin, double xMax, double yMin, double yMax);
	
	Point<int> GetScreenCoords(double x, double y) const;
	Point<int> GetScreenCoords(Point<double> point) const;
	Point<double> GetGraphCoords(int x, int y) const;
	Point<double> GetGraphCoords(Point<int> point) const;
	
	void Pan(double x, double y);
	void ZoomIn(double factor);
	void ZoomOut(double factor);

	int GetOffset() const;
	void ResetOffset();
};
