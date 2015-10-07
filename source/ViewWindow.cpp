#include "ViewWindow.h"

ViewWindow::ViewWindow(float xMin, float xMax, float yMin, float yMax)
{
	xmin = xMin;
	xmax = xMax;
	ymin = yMin;
	ymax = yMax;
}

Point<int> ViewWindow::GetScreenCoords(float x, float y) const
{
	int sx = (int)Interpolate(x, xmin, xmax, sxmin, sxmax);
	int sy = (int)Interpolate(y, ymin, ymax, symin, symax);
	return Point<int>(sx, sy);
}

Point<int> ViewWindow::GetScreenCoords(Point<float> point) const
{
	return GetScreenCoords(point.x, point.y);
}

Point<float> ViewWindow::GetGraphCoords(int x, int y) const
{
	float gx = Interpolate((float)x, sxmin, sxmax, xmin, xmax);
	float gy = Interpolate((float)y, symin, symax, ymin, ymax);
	return Point<float>(gx, gy);
}

Point<float> ViewWindow::GetGraphCoords(Point<int> point) const
{
	return GetGraphCoords(point.x, point.y);
}

void ViewWindow::Pan(float x, float y)
{
	xmin += x;
	xmax += x;
	ymin += y;
	ymax += y;
}

void ViewWindow::ZoomIn(float factor)
{
	ZoomOut(1.0f / factor);
}

void ViewWindow::ZoomOut(float factor)
{
	float centerX = (xmin + xmax) / 2;
	float centerY = (ymin + ymax) / 2;
	xmin = Interpolate(factor, 1.0f, 0.0f, xmin, centerX);
	xmax = Interpolate(factor, 1.0f, 0.0f, xmax, centerX);
	ymin = Interpolate(factor, 1.0f, 0.0f, ymin, centerY);
	ymax = Interpolate(factor, 1.0f, 0.0f, ymax, centerY);
}