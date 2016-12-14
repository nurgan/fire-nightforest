#include "Shape.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "Material.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
   vaoID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

void Shape::loadMesh(const string &meshName, unsigned int meshNum)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		posBuf = shapes[meshNum].mesh.positions;
		norBuf = shapes[meshNum].mesh.normals;
		texBuf = shapes[meshNum].mesh.texcoords;
		eleBuf = shapes[meshNum].mesh.indices;
	}

}

void Shape::computeNormals(){
   unsigned int i0, i1, i2;
   vec3 v0, v1, v2, u, w, n, nn;

   std::vector<float> norTmp (posBuf.size(), 0.0);

   //go over each face = 3  per elebuf1
   for(int i = 0; i < eleBuf.size(); i = i + 3){
      i0 = eleBuf[i]*3;
      i1 = eleBuf[i+1]*3;
      i2 = eleBuf[i+2]*3;

      v0 = vec3(posBuf[i0], posBuf[i0+1], posBuf[i0+2]);
      v1 = vec3(posBuf[i1], posBuf[i1+1], posBuf[i1+2]);
      v2 = vec3(posBuf[i2], posBuf[i2+1], posBuf[i2+2]);

      u = v1 - v0;
      w = v2 - v0;

      n = cross(u, w);

      norTmp[i0] = norTmp[i0] + n.x;      
      norTmp[i0+1] = norTmp[i0+1] + n.y;
      norTmp[i0+2] = norTmp[i0+2] + n.z;

      norTmp[i1] = norTmp[i1] + n.x;      
      norTmp[i1+1] = norTmp[i1+1] + n.y;
      norTmp[i1+2] = norTmp[i1+2] + n.z;

      norTmp[i2] = norTmp[i2] + n.x;      
      norTmp[i2+1] = norTmp[i2+1] + n.y;
      norTmp[i2+2] = norTmp[i2+2] + n.z;
          
   }
   
   for(int i = 0; i < norTmp.size(); i = i + 3){
      n = vec3(norTmp[i], norTmp[i+1], norTmp[i+2]);
      nn = normalize(n);
      norTmp[i] = nn.x;
      norTmp[i+1] = nn.y;
      norTmp[i+2] = nn.z;
   }

   norBuf = norTmp;

}

vector<float> Shape::getMinMax(){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

   vector<float> minMax;
   minMax.push_back(minX);
   minMax.push_back(maxX);
   minMax.push_back(minY);
   minMax.push_back(maxY);
   minMax.push_back(minZ);
   minMax.push_back(maxZ);

   return minMax;
}

void Shape::resize() {
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	//From min and max compute necessary scale and shift for each dimension
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

   //Go through all verticies shift and scale them
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
		assert(posBuf[3*v+0] >= -1.0 - epsilon);
		assert(posBuf[3*v+0] <= 1.0 + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
		assert(posBuf[3*v+1] >= -1.0 - epsilon);
		assert(posBuf[3*v+1] <= 1.0 + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
		assert(posBuf[3*v+2] >= -1.0 - epsilon);
		assert(posBuf[3*v+2] <= 1.0 + epsilon);
	}
}

void Shape::resize(float scaleX, float scaleY, float scaleZ, float shiftX, float shiftY, float shiftZ) {
   float epsilon = 0.001;   
   
   //Go through all verticies shift and scale them
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
		assert(posBuf[3*v+0] >= -1.0 - epsilon);
		assert(posBuf[3*v+0] <= 1.0 + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
		assert(posBuf[3*v+1] >= -1.0 - epsilon);
		assert(posBuf[3*v+1] <= 1.0 + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
		assert(posBuf[3*v+2] >= -1.0 - epsilon);
		assert(posBuf[3*v+2] <= 1.0 + epsilon);
	}
}

void Shape::init()
{
   // Initialize the vertex array object
   glGenVertexArrays(1, &vaoID);
   glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	if(norBuf.empty()) {
		norBufID = 0;
	} else {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the texture array to the GPU
	if(texBuf.empty()) {
		texBufID = 0;
	} else {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
	
	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

   glBindVertexArray(vaoID);
	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}

	if (texBufID != 0) {	
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if(h_tex != -1 && texBufID != 0) {
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}
	
	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
	
	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shape::draw(const shared_ptr<Program> prog, Material &material) const
{
   //set Material Information
   glUniform3f(prog->getUniform("ambientMatColor"), material.getAmbient()[0], material.getAmbient()[1], material.getAmbient()[2]);
   glUniform3f(prog->getUniform("diffuseMatColor"), material.getDiffuse()[0], material.getDiffuse()[1], material.getDiffuse()[2]);
   glUniform3f(prog->getUniform("specularMatColor"), material.getSpecular()[0], material.getSpecular()[1], material.getSpecular()[2]);
   glUniform1f(prog->getUniform("shininess"), material.getShininess());

   draw(prog);


}
