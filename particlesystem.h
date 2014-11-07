#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__
#include <list>
#include "Vector3.h"

class Particle
{ 
public:
	enum Shape { Sphere, Cube, Teapot };
	Vector3 position;
	Vector3 direction;
	Vector3 rotation;
	Vector3 color;
	Vector3 velocity;
	Vector3 spin;
	float speed;
	float size;
	float age;
	float friction;
	Shape shape;
	Particle(void);
};

class Cannon
{
public:
	Vector3 positionBase;
	Vector3 positionSpout;
	Vector3 direction;
	float spread;
	Cannon(void);
	Cannon(Vector3, Vector3, Vector3, float);
};

class ParticleSystem
{
private:
	std::list<Particle> list;
public:
	enum State { Falling, Exploding};
	typedef std::list<Particle>::iterator iter;
	ParticleSystem(void);
	iter begin();
	iter end();
	void createParticle(float num, Cannon cannon);
	void update(float deltaTime);
	void append(Particle);
	void setState(State);
	void clear(void);
	bool hasFriction;
	bool hasGravity;
	bool hasWind;
	float gravity;
	float wind;
	int maxAge;
	State state;
	int particleRotationAngle;
};

#endif