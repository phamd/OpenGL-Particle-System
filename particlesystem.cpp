#include "ParticleSystem.h"

Particle::Particle(void)
{
	position = direction = rotation = color = velocity = spin = Vector3();
	speed = size = 1;
	age = 0;
	shape = Shape::Sphere;
}

ParticleSystem::ParticleSystem(void)
{
	state = Falling;
	hasFriction = false;
	hasGravity = true;
	//hasSpin = true;
	maxAge = 150;
	gravity = -0.3;
}

void ParticleSystem::update(float deltaTime) // note: maybe just update in main.cpp instead
{
	particleRotationAngle = (particleRotationAngle > 360) ? 0 : particleRotationAngle+deltaTime ;


	deltaTime = deltaTime / 50; // scaling the speed down

	//printf("%d\n",deltaTime);
	for (auto it = begin(); it != end(); ) {
		
		// update position
		if (hasGravity) {
			float accel = gravity;
			it->position.y += (it->velocity.y + accel / 2) * deltaTime;
			it->velocity.y += accel;
			it->velocity.y *= 0.90;
		}
		else {
			it->position.y += it->velocity.y * deltaTime;
		}
		it->position.x += it->velocity.x * deltaTime;
		it->position.z += it->velocity.z * deltaTime;
		
		if (abs(it->position.x) < 50 && abs(it->position.z) < 50  && it->position.y < 1) {
			it->position.y = 1.000001;
			it->velocity.y = (abs(it->velocity.y) < 0.9) ? 0 : it->velocity.y; // stops flickering of bounces
			it->velocity.y = abs(it->velocity.y);
			if (hasFriction) {
				it->velocity.x *= 0.10;
				it->velocity.z *= 0.10;
			}
		}
		
		/// update age; (age > xx) ? kill.
		if (it->age > maxAge || it->position.y < -10) {
			it = list.erase(it);
		}
		else {
			it->age += deltaTime;
			++it;
		}
	}
}

void ParticleSystem::append(Particle p)
{
	list.push_back(p);
}

void ParticleSystem::clear(void)
{
	list.clear();
}
ParticleSystem::iter ParticleSystem::begin(void)
{
	return list.begin();
}

ParticleSystem::iter ParticleSystem::end(void)
{
	return list.end();
}