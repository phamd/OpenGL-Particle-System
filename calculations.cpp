#include "calculations.h"
#include <cstdlib>

float randFloat(float s, float t)
{
	return s + (t - s) * ((float)rand()) / (float)RAND_MAX;
}

float absf(float value)
{
	return (value >= 0) ? value : -value;
}