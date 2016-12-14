//
// sueda
// November, 2014/ wood 16
//

#pragma once
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Perlin.h"
#include "FractalNoise.h"

using namespace glm;

class MatrixStack;
class Program;
class Texture;

class Particle
{
public:
	Particle(vec3 spawnCenter);
	virtual ~Particle();
	void load();
	void rebirth(float t);
	//void update(float t, float h, const glm::vec3 &g, const bool *keyToggles, Perlin& perlin);
   void update(float t, float h, const glm::vec3 &g, const bool *keyToggles, FractalNoise* fNoise);
	const vec3 &getPosition() const { return x; };
	const vec3 &getVelocity() const { return v; };
	const vec4 &getColor() const { return color; };
   float getLifePercent(float t);
	
private:
   vec3 center;
   vec3 narrowMove;
	float charge; // +1 or -1
	float m; // mass
	float d; // viscous damping
	vec3 x; // position
	vec3 v; // velocity
   vec3 w; // wind
	float lifespan; // how long this particle lives
	float tEnd;     // time this particle dies
	float scale;
	vec4 color;
};

#endif
