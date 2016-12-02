#include "Common.h"

double Interpolate(double value, double inputMin, double inputMax, double outputMin, double outputMax)
{
	double x = (value - inputMin) / (inputMax - inputMin);
	return x * (outputMax - outputMin) + outputMin;
}

std::string vssprintf(const char *format, va_list arg)
{
    int size = vsnprintf(nullptr, 0, format, arg);
    char *buf = new char[size + 1];
    vsnprintf(buf, size + 1, format, arg);
    std::string str = buf;
    delete[] buf;
    return str;
}

std::string ssprintf(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    std::string str = vssprintf(format, vl);
    va_end(vl);
    return str;
}