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
	float spread; // The random adjustment to the angle of particles leaving the spout.
	float radius; // The radius of the spout; e.g. a rain cloud would be given a large radius.
	Cannon(void);
	Cannon(Vector3, Vector3, float, float);
	Vector3 getDirection(void);
};

class ParticleSystem
{
private:
	std::list<Particle> list;
public:
	//enum State { Falling, Exploding };
	typedef std::list<Particle>::iterator iter;
	ParticleSystem(void);
	iter begin();
	iter end();
	void createParticle(int flowRate, Cannon cannon); // There can be many cannons in a system.
	void update(float deltaTime); // All the particle math is done here
	void append(Particle);
	//void setState(State);
	void clear(void);
	bool hasFriction;
	bool hasGravity;
	bool hasWind;
	bool cubesOnly; // render speed is much faster with cubes only
	float gravity;
	float wind;
	int maxAge;
	//State state;
	int particleRotationAngle;
};

#endif