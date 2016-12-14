#include "Material.h"

using namespace std;

Material::Material() :
	ambient(0),
	diffuse(0),
	specular(0),
	shininess(0)
{
}

Material::~Material()
{
}

void Material::setDiffuse(float r, float g, float b){
   diffuse.push_back(r);
   diffuse.push_back(g);
   diffuse.push_back(b);
}

void Material::setAmbient(float r, float g, float b){
   ambient.push_back(r);
   ambient.push_back(g);
   ambient.push_back(b);
}

void Material::setSpecular(float r, float g, float b){
   specular.push_back(r);
   specular.push_back(g);
   specular.push_back(b);
}

void Material::setShininess(float s){
   shininess = s;
}

vector<float> Material::getAmbient(){
   return ambient;
} 

vector<float> Material::getDiffuse(){
   return diffuse;
} 

vector<float> Material::getSpecular(){
   return specular;
} 

float Material::getShininess(){
   return shininess;
}
