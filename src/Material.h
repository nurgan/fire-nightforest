#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include <vector>
#include <memory>

class Material
{
public:
	Material();
	virtual ~Material();
	void setDiffuse(float r, float g, float b);
   void setAmbient(float r, float g, float b);
   void setSpecular(float r, float g, float b);
   void setShininess(float s);
   std::vector<float> getAmbient(); 
   std::vector<float> getDiffuse();
   std::vector<float> getSpecular();
   float getShininess();
	
private:
	std::vector<float> ambient;
	std::vector<float> diffuse;
	std::vector<float> specular;
   float shininess;
};

#endif
