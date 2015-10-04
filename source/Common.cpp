#include "Common.h"

float Interpolate(float value, float inputMin, float inputMax, float outputMin, float outputMax)
{
	float x = (value - inputMin) / (inputMax - inputMin);
	return x * (outputMax - outputMin) + outputMin;
}