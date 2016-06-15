#pragma once
#include <string>
#include <cstdio>
#include <cstdarg>
#include <sf2d.h>

template <typename _T>
struct Point
{
	_T x, y;
	
	Point()
	{
		x = y = _T();
	}
	
	Point(_T x, _T y)
	{
		this->x = x;
		this->y = y;
	}
	
	template <typename _NewType>
	operator Point<_NewType>() const
	{
		return Point<_NewType>(x, y);
	}
};

template <typename _T>
struct Rect
{
	_T x, y, w, h;
	
	Rect()
	{
		x = y = w = h = _T();
	}
	
	Rect(_T x, _T y, _T w, _T h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	
	bool PointInside(_T x, _T y) const
	{
		return (x >= this->x) && (y >= this->y) && (x < this->x + w) && (y < this->y + h);
	}
	
	bool PointInside(Point<_T> point) const
	{
		return PointInside(point.x, point.y);
	}
	
	template <typename _NewType>
	operator Rect<_NewType>() const
	{
		return Rect<_NewType>(x, y, w, h);
	}
};

float Interpolate(float value, float inputMin, float inputMax, float outputMin, float outputMax);

std::string vssprintf(const char *format, va_list arg);
std::string ssprintf(const char *format, ...);