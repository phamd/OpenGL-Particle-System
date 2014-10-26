#include "particlesystem.h"

Particle::Particle(void) {
	position = direction = rotation = color = velocity = Vector3();
	speed = size = 1;
	age = 0;
	shape = Shape::Sphere;
}

ParticleSystem::ParticleSystem(void) {
	hasFriction = true;
}

void ParticleSystem::update(int deltaTime) { // note: maybe just update in main.cpp instead
	deltaTime = deltaTime/10;
	//printf("%d\n",deltaTime);
	for (auto it = begin(); it != end(); it++) {
		float accel = -0.2* deltaTime;
		// update position
		it->velocity.y += accel * deltaTime;
		it->position.x += it->velocity.x * deltaTime;
		it->position.y += accel * deltaTime;
		it->position.z += it->velocity.z * deltaTime;
		//it->position.add(it->direction);
		if (it->position.y < 1) {
			it->position.y = 1.0001;
			it->velocity.y = -it->velocity.y;
			// bounce
			//it->velocity.x = -it->velocity.x;
			//it->velocity.z = -it->velocity.z;
			if (hasFriction) {
				it->velocity.x *= 0.95;
				it->velocity.z *= 0.95;
			}
			
		}

		// update direction based on gravity, wind

		// update age; (age > xx) ? kill.

	}
}

void ParticleSystem::append(Particle p) {
	list.push_back(p);
}

ParticleSystem::iter ParticleSystem::begin(void) {
	return list.begin();
}

ParticleSystem::iter ParticleSystem::end(void) {
	return list.end();
}