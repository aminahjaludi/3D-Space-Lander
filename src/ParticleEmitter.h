#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "TransformObject.h"
#include "ParticleSystem.h"

typedef enum { DirectionalEmitter, RadialEmitter, SphereEmitter, DiskEmitter } EmitterType;

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class ParticleEmitter : public TransformObject {
public:
	ParticleEmitter();
	ParticleEmitter(ParticleSystem* s);
	~ParticleEmitter();
	void init();
	void draw();
	void start();
	void stop();
	void setLifespan(const float life) { lifespan = life; }
	void setVelocity(const ofVec3f& vel) { velocity = vel; }
	void setRate(const float r) { rate = r; }
	void setParticleRadius(const float r) { particleRadius = r; }
	void setEmitterType(EmitterType t) { type = t; }
	void setGroupSize(int s) { groupSize = s; }
	void setOneShot(bool s) { oneShot = s; }
	void update();
	void spawn(float time);
	ParticleSystem* sys;
	float rate;         // per sec
	bool oneShot;
	bool fired;
	ofVec3f velocity;
	float lifespan;     // sec
	bool started;
	float lastSpawned;  // ms
	float particleRadius;
	float radius;
	bool visible;
	int groupSize;      // number of particles to spawn in a group
	bool createdSys;
	EmitterType type;
};


ParticleEmitter::ParticleEmitter() {
	sys = new ParticleSystem();
	createdSys = true;
	init();
}

ParticleEmitter::ParticleEmitter(ParticleSystem* s) {
	if (s == NULL)
	{
		cout << "fatal error: null particle system passed to ParticleEmitter()" << endl;
		ofExit();
	}
	sys = s;
	createdSys = false;
	init();
}

ParticleEmitter::~ParticleEmitter() {

	// deallocate particle system if emitter created one internally
	//
	if (createdSys) delete sys;
}

void ParticleEmitter::init() {
	rate = 1;
	velocity = ofVec3f(0, 20, 0);
	lifespan = 3;
	started = false;
	oneShot = false;
	fired = false;
	lastSpawned = 0;
	radius = 0.1;
	particleRadius = .1;
	visible = true;
	type = DiskEmitter;
	groupSize = 1;
}



void ParticleEmitter::draw() {
	if (visible) {
		switch (type) {
		case DirectionalEmitter:
			ofDrawSphere(position, radius / 10);  // just draw a small sphere for point emitters 
			break;
		case SphereEmitter:
		case RadialEmitter:
		case DiskEmitter:
			ofDrawSphere(position, radius / 10);  // just draw a small sphere as a placeholder
			break;
		default:
			break;
		}
	}
	sys->draw();
}
void ParticleEmitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void ParticleEmitter::stop() {
	started = false;
	fired = false;
}
void ParticleEmitter::update() {

	float time = ofGetElapsedTimeMillis();

	if (oneShot && started) {
		if (!fired) {

			// spawn a new particle(s)
			//
			for (int i = 0; i < groupSize; i++)
				spawn(time);

			lastSpawned = time;
		}
		fired = true;
		stop();
	}

	else if (((time - lastSpawned) > (1000.0 / rate)) && started) {

		// spawn a new particle(s)
		//
		for (int i = 0; i < groupSize; i++)
			spawn(time);

		lastSpawned = time;
	}

	sys->update();
}

// spawn a single particle.  time is current time of birth
//
void ParticleEmitter::spawn(float time) {

	Particle particle;

	// set initial velocity and position
	// based on emitter type
	//
	switch (type) {
	case DiskEmitter:
	{
		//need to create a random x and z components for vector within 'disk' radius
		//use unit circle measurements for the angle and radius offset
		float randangle = ofRandom(0, TWO_PI);
		//create a random value for the radius offset for the x component and z component
		float randradiusOffset = ofRandom(0, radius * 3);

		//calculate the x y and z components of the particle's position
		float x = cos(randangle) * randradiusOffset;
		float z = sin(randangle) * randradiusOffset;

		//clamp the y component of the particle to stay in bounds of the disk
		//float y = ofClamp(position.y, position.y - radius, position.y + radius);

		float y = position.y;

		float speed = velocity.length() * ofRandom(0.1, 0.3);  // Reduce spread further
		particle.velocity = ofVec3f(x * speed, -abs(velocity.y) * ofRandom(0.1, 0.3), z * speed);  // Limit upward force
		//set particle's position and velocity
		particle.position.set(position.x + x, y, position.z + z);
	}
	break;
	case RadialEmitter:
	{
		ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
		float speed = velocity.length();
		particle.velocity = dir.getNormalized() * speed;
		particle.position.set(position);
	}
	break;
	case SphereEmitter:
		break;
	case DirectionalEmitter:
		particle.velocity = velocity;
		particle.position.set(position);
		break;

	}

	// other particle attributes
	//
	particle.lifespan = lifespan;
	particle.birthtime = time;
	particle.radius = particleRadius;

	// add to system
	//
	sys->add(particle);
}
