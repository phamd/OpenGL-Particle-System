#include "Vector3.h"
#include <cstdlib>
#include <math.h>

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(void)
{
	x = y = z = 0;
}

Vector3 Vector3::directionTo(Vector3 other)
{
	Vector3 delta = { other.x - x, other.y - y, other.z - z };
	delta.x = (delta.x == 0) ? 0 : delta.x / delta.length();
	delta.y = (delta.y == 0) ? 0 : delta.y / delta.length();
	delta.z = (delta.z == 0) ? 0 : delta.z / delta.length();
	return delta;
}

void Vector3::randomize(void)
{
	x = (float)rand() / float(RAND_MAX);
	y = (float)rand() / float(RAND_MAX);
	z = (float)rand() / float(RAND_MAX);
}

void Vector3::add(Vector3 other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

void Vector3::scale(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

float Vector3::dot(Vector3 other)
{
	return x*other.x + y*other.y + z*other.z;
}

float Vector3::length(void)
{
	return sqrt(x*x + y*y + z*z);
}

float Vector3::angle(Vector3 other)
{
	return this->dot(other) / (abs(this->length())*(abs(other.length())));
}