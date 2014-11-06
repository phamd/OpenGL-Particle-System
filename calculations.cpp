#include "calculations.h"
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <time.h>

float randFloat(float s, float t) {
	return s + (t - s) * ((float)rand()) / (float)RAND_MAX;
}
