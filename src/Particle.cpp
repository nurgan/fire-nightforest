//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "texture.h"


using namespace std;

float randFloat(float l, float h)
{
	float r = rand() / (float)RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 spawnCenter) :
   center(spawnCenter),
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(0.0f, 0.0f, 0.0f),
	v(0.0f, 0.0f, 0.0f),
	lifespan(1.0f),
	tEnd(0.0f),
	scale(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Particle::~Particle()
{
}

void Particle::load()
{
	// Random initialization
	rebirth(0.0f);
}

float Particle::getLifePercent(float t)
{
   return 1.0 - ((tEnd-t)/lifespan);
}

/* all particles born at the origin */
void Particle::rebirth(float t)
{

	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
  	d = randFloat(0.0f, 0.02f);
   x.x = randFloat(center.x - 0.5, center.x + 0.5); 
   x.y = center.y; 
   x.z = randFloat(center.z - 0.5, center.z + 0.5);

   v.x = 0.0;
	v.y = randFloat(1.0f, 2.5f);
   v.z = 0.0;

   lifespan = randFloat(1.0f, 3.0f); 


   narrowMove = center - x;
   narrowMove = vec3(narrowMove.x, 0.0, narrowMove.z);
   narrowMove /= 200.0;

   w = vec3(0.01, 0.0, 0.0);
     

	tEnd = t + lifespan;
	scale = randFloat(0.2, 1.0f);

   color.r = 1.0;
   color.g = 1.0;
   color.b = 0.0;

	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const bool *keyToggles, FractalNoise* fNoise)
{
	if(t > tEnd) {
		rebirth(t);
	}

   float noise = fNoise->noise(x.x, x.y, x.z);
   float noiseZ = fNoise->noise(-x.x, -x.y, -x.z);

   noiseZ /= 10.0;
   noiseZ *= 1.0 - (tEnd-t)/lifespan;
   noise /= 10.0;
   noise *= 1.0 - (tEnd-t)/lifespan;

   x += h * v;
   
   x += narrowMove;
   
   x += vec3(noise, 0, noiseZ);

   w = vec3(w.x, 0.001 * sin(t), 0.0);

   v += w;


   if(((tEnd-t)/lifespan) < 0.5){
      color.r = 0.4;
      color.g = 0.4;
      color.b = 0.4;      
   } else if(((tEnd-t)/lifespan) < 0.95){
      color.g = (tEnd-t)/(lifespan * 2.0);
   }

	color.a = (tEnd-t)/lifespan;
}
