#include "ParticleSystem.h"
#include "calculations.h"

Particle::Particle(void)
{
	position = direction = rotation = color = velocity = spin = Vector3();
	speed = size = 1;
	friction = 0.8;
	age = 0;
	shape = Shape::Sphere;
}

ParticleSystem::ParticleSystem(void)
{
	state = Falling;
	hasFriction = false;
	hasGravity = true;
	hasWind = false;
	gravity = -0.3;
	wind = 0.5;
	maxAge = 150;
}

void ParticleSystem::update(float deltaTime) // note: maybe just update in main.cpp instead
{
	deltaTime = deltaTime / 50; // scaling the speed down

	// particle rotation angle
	particleRotationAngle = (particleRotationAngle >= 350) ? 0 : (particleRotationAngle+10);
	
	for (auto it = begin(); it != end(); ) {
		// update position
		if (hasGravity) { // move y
			float accel = gravity;
			it->position.y += (it->velocity.y + accel / 2) * deltaTime;
			it->velocity.y += accel;
			it->velocity.y *= 0.90;
		}
		else { // move y
			it->position.y += it->velocity.y * deltaTime;
		} // move x,z
		it->position.x += (hasWind) ? (it->velocity.x + wind) * deltaTime : it->velocity.x * deltaTime;
		it->position.z += (hasWind) ? (it->velocity.z + wind) * deltaTime : it->velocity.z * deltaTime;
		
		// hardcoded floor collision and bouncing
		if (abs(it->position.x) < 50 && abs(it->position.z) < 50  && it->position.y < 1) {
			it->position.y = 1.000001;
			it->velocity.y = (abs(it->velocity.y) < 0.9) ? 0 : it->velocity.y; // stops flickering of bounces
			it->velocity.y = abs(it->velocity.y); // reverse direction for bounce
			if (hasFriction) { // hardcoded friction values
				it->velocity.x *= it->friction;
				it->velocity.z *= it->friction;
			}
		}
		// update age
		if (it->age > maxAge || it->position.y < -10) {
			it = list.erase(it);
		}
		else {
			it->age += deltaTime;
			++it;
		}
	}
}


void ParticleSystem::createParticle(float num, Cannon cannon)
{
	for (int i = 0; i < num; i++) {
		Particle p = Particle();
		p.size = randFloat(0.01, 1);
		p.speed = randFloat(0.5, 2);
		p.position = cannon.direction;
		p.position.x += randFloat(-0.1, 0.1);
		p.position.z += randFloat(-0.1, 0.1);
		p.direction = cannon.positionBase.directionTo(
		{ cannon.direction.x + randFloat(-cannon.spread, cannon.spread),
		cannon.direction.y,
		cannon.direction.z + randFloat(-cannon.spread, cannon.spread) }
		);
		p.velocity = p.direction;
		p.velocity.scale(p.size * p.speed); // bigger shapes bounce farther
		p.color.randomize();
		p.spin.randomize();
		p.shape = (randFloat(0, 1) < 0.50) ? Particle::Sphere : (randFloat(0, 1) < 0.80) ?
			Particle::Cube : Particle::Teapot; // (0.5-0.5*0.8)% chance of teapot
		list.push_back(p); // Add to particle list
	}
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

Cannon::Cannon(void){}

Cannon::Cannon(Vector3 positionBase, Vector3 positionSpout, Vector3 direction, float spread)
{
	this->positionBase = positionBase;
	this->positionSpout = positionSpout;
	this->direction = direction;
	this->spread = spread;
}