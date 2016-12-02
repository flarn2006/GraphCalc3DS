#include "ViewWindow.h"

ViewWindow::ViewWindow(double xMin, double xMax, double yMin, double yMax)
{
	xmin = xMin;
	xmax = xMax;
	ymin = yMin;
	ymax = yMax;
}

Point<int> ViewWindow::GetScreenCoords(double x, double y) const
{
	int sx = (int)Interpolate(x, xmin, xmax, sxmin, sxmax);
	int sy = (int)Interpolate(y, ymin, ymax, symin, symax);
	return Point<int>(sx, sy);
}

Point<int> ViewWindow::GetScreenCoords(Point<double> point) const
{
	return GetScreenCoords(point.x, point.y);
}

Point<double> ViewWindow::GetGraphCoords(int x, int y) const
{
	double gx = Interpolate((double)x, sxmin, sxmax, xmin, xmax);
	double gy = Interpolate((double)y, symin, symax, ymin, ymax);
	return Point<double>(gx, gy);
}

Point<double> ViewWindow::GetGraphCoords(Point<int> point) const
{
	return GetGraphCoords(point.x, point.y);
}

void ViewWindow::Pan(double x, double y)
{
	xmin += x;
	xmax += x;
	ymin += y;
	ymax += y;
}

void ViewWindow::ZoomIn(double factor)
{
	ZoomOut(1.0 / factor);
}

void ViewWindow::ZoomOut(double factor)
{
	double centerX = (xmin + xmax) / 2;
	double centerY = (ymin + ymax) / 2;
	xmin = Interpolate(factor, 1.0, 0.0, xmin, centerX);
	xmax = Interpolate(factor, 1.0, 0.0, xmax, centerX);
	ymin = Interpolate(factor, 1.0, 0.0, ymin, centerY);
	ymax = Interpolate(factor, 1.0, 0.0, ymax, centerY);
}