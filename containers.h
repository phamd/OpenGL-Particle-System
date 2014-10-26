#ifndef __CONTAINERS_H__
#define __CONTAINERS_H__

class Vector3
{
public:
	union { // union of x,y,z to array v
		struct {
			float x, y, z;
		}; // anonymous struct
		float v[3]; // will be used for glcolor3fv(v);
	}; // anonymous union
	Vector3(float, float, float);
	Vector3(void);
	Vector3 directionTo(Vector3);
	void add(Vector3);
	void scale(float);
	float dot(Vector3);
};

#endif