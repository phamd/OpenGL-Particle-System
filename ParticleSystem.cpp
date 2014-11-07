#include "ParticleSystem.h"
#include "calculations.h"

Particle::Particle(void)
{
	position = direction = rotation = color = velocity = spin = Vector3();
	speed = size = 1;
	friction = 0.8f;
	age = 0;
	shape = Shape::Sphere;
}

ParticleSystem::ParticleSystem(void)
{
	//state = Falling;
	hasFriction = false;
	hasGravity = true;
	hasWind = false;
	gravity = -0.3f;
	wind = 0.5f;
	maxAge = 150;
	cubesOnly = false;
}

void ParticleSystem::update(float deltaTime)
{
	deltaTime = deltaTime / 50; // scaling the speed down

	// particle rotation angle (individual particles still spin at their own speeds)
	particleRotationAngle = (particleRotationAngle >= 350) ? 0 : (particleRotationAngle+10);

	for (auto it = begin(); it != end(); ) {
		// update position
		if (hasGravity) { // move y
			float accel = gravity;
			it->position.y += (it->velocity.y + accel / 2) * deltaTime;
			it->velocity.y += accel;
			it->velocity.y *= 0.90f;
		}
		else { // move y
			it->position.y += it->velocity.y * deltaTime;
		} // move x,z
		it->position.x += (hasWind) ? (it->velocity.x + wind) * deltaTime : it->velocity.x * deltaTime;
		it->position.z += (hasWind) ? (it->velocity.z + wind) * deltaTime : it->velocity.z * deltaTime;

		// hardcoded floor collision and bouncing
		if (absf(it->position.x) < 50 && absf(it->position.z) < 50  && it->position.y < 1) {
			// checks current position, not next; otherwise particles bounce off the air.
			it->position.y = 1.000001f; // nudge up slightly, so it can bounce up
			it->velocity.y = (absf(it->velocity.y) < 0.9) ? 0 : it->velocity.y; // stops flickering of bounces
			it->velocity.y = absf(it->velocity.y); // reverse direction for bounce
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

void ParticleSystem::createParticle(int flowRate, Cannon cannon)
{
	for (int i = 0; i < flowRate; i++) {
		Particle p = Particle();
		p.size = randFloat(0.1f, 1);
		p.speed = randFloat(0.5f, 2);
		p.position = cannon.positionSpout;
		p.position.x += randFloat(-cannon.radius, cannon.radius);
		p.position.z += randFloat(-cannon.radius, cannon.radius);
		p.direction = cannon.positionBase.directionTo(
		{ cannon.positionSpout.x + randFloat(-cannon.spread, cannon.spread),
		cannon.positionSpout.y,
		cannon.positionSpout.z + randFloat(-cannon.spread, cannon.spread) }
		);
		p.velocity = p.direction;
		p.velocity.scale(p.size * p.speed); // bigger shapes bounce farther
		p.color.randomize();
		p.spin.randomize();
		if (cubesOnly) {
			p.shape = Particle::Cube;
		} else {
			p.shape = (randFloat(0, 1) < 0.50f) ? Particle::Sphere : (randFloat(0, 1) < 0.80f) ?
			Particle::Cube : Particle::Teapot; // (0.5-0.5*0.8)% chance of teapot
		}
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

Cannon::Cannon(void)
{
	positionBase = positionSpout = { 0, 0, 0 };
	spread = radius = 1;
}

Cannon::Cannon(Vector3 positionBase, Vector3 positionSpout, float spread, float spoutRadius)
{
	this->positionBase = positionBase;
	this->positionSpout = positionSpout;
	this->spread = spread;
	this->radius = spoutRadius;
}