#pragma once

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

float Interpolate(float value, float inputMin, float inputMax, float outputMin, float outputMax);