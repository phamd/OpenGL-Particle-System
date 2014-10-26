#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__
#include <list>
#include "containers.h"

class Particle {
public:
	enum Shape { Sphere, Cube, Teapot };
	Vector3 position;
	Vector3 direction;
	Vector3 rotation;
	Vector3 color;
	Vector3 velocity;
	float speed;
	float size;
	float age;
	Shape shape;
	Particle(void);
};

class ParticleSystem {
private:
	std::list<Particle> list;
public:
	typedef std::list<Particle>::iterator iter;
	ParticleSystem(void);
	iter begin();
	iter end();
	void update(int deltaTime);
	void append(Particle);
	bool hasFriction;
};

#endif