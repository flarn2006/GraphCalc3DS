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
	xmin /= factor;
	xmax /= factor;
	ymin /= factor;
	ymax /= factor;
}

void ViewWindow::ZoomOut(float factor)
{
	xmin *= factor;
	xmax *= factor;
	ymin *= factor;
	ymax *= factor;
}