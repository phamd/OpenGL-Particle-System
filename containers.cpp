#include "containers.h"
#include <math.h>

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(void) {
	x = y = z = 0;
}

Vector3 Vector3::directionTo(Vector3 f) {
	float deltaX = f.x - x;
	float deltaY = f.y - y;
	float deltaZ = f.z - z;
	float length = sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
	return Vector3((deltaX == 0) ? 0 : deltaX / length,
				   (deltaY == 0) ? 0 : deltaY / length,
				   (deltaZ == 0) ? 0 : deltaZ / length);
}

void Vector3::add(Vector3 other) {
	x += other.x;
	y += other.y;
	z += other.z;
}

void Vector3::scale(float scale) {
	x *= scale;
	y *= scale;
	z *= scale;
}

float Vector3::dot(Vector3 other) {
	return x*other.x + y*other.y + z*other.z;
}