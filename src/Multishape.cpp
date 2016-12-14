#include "Multishape.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"

#include "tiny_obj_loader.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

Multishape::Multishape()
{
}

Multishape::~Multishape()
{
}

void Multishape::loadMesh(const string &meshName)
{
	vector<tinyobj::shape_t> s;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(s, objMaterials, errStr, meshName.c_str());
   
     
   if(!rc) {
		cerr << errStr << endl;
	} else {
      
		int numShapes = s.size();
      for(int i = 0; i < numShapes; i++){
         Shape shape;
         shape.loadMesh(meshName, i);
         shapes.push_back(shape);
      }
	}
}

void Multishape::loadMesh(const string &meshName, int min, int max)
{
	vector<tinyobj::shape_t> s;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(s, objMaterials, errStr, meshName.c_str());
   
   if(!rc) {
		cerr << errStr << endl;
	} else {
      
		int numShapes = s.size();
      for(int i = min; i <= max; i++){
         Shape shape;
         shape.loadMesh(meshName, i);
         shapes.push_back(shape);
      }
      
	}
   
}

void Multishape::resize() {
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   for(int i = 0; i < shapes.size(); i++){
      vector<float> mm = shapes[i].getMinMax();
      if(mm[0] < minX) minX = mm[0];
		if(mm[1] > maxX) maxX = mm[1];

		if(mm[2] < minY) minY = mm[2];
		if(mm[3] > maxY) maxY = mm[3];

		if(mm[4] < minZ) minZ = mm[4];
		if(mm[5] > maxZ) maxZ = mm[5];
   }

   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float maxExtent, xExtent, yExtent, zExtent;

   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   for(int i = 0; i < shapes.size(); i++){
      shapes[i].resize(scaleX, scaleY, scaleZ, shiftX, shiftY, shiftZ);
   }
}

void Multishape::init()
{
    for(int i = 0; i < shapes.size(); i++){
         shapes[i].init();
    }
}

void Multishape::draw(const shared_ptr<Program> prog) const
{
    for(int i = 0; i < shapes.size(); i++){
         shapes[i].draw(prog);
    }
}

void Multishape::draw(const std::shared_ptr<Program> prog, vector<Material> materials) const
{
    for(int i = 0; i < shapes.size(); i++){
         shapes[i].draw(prog, materials[i]);
    }
}

void Multishape::draw(const std::shared_ptr<Program> prog, Material material) const
{
    for(int i = 0; i < shapes.size(); i++){
         shapes[i].draw(prog, material);
    }
}
