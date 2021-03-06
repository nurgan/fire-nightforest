#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>

class Program;
class Material;

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void loadMesh(const std::string &meshName);
   void loadMesh(const std::string &meshName, unsigned int meshNum);
   void computeNormals();
	void init();
	void resize();
   void resize(float scaleX, float scaleY, float scaleZ, float shiftX, float shiftY, float shiftZ);
	void draw(const std::shared_ptr<Program> prog) const;
   void draw(const std::shared_ptr<Program> prog, Material &material) const;
   std::vector<float> getMinMax();
	
private:
	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;
   unsigned vaoID;
};

#endif
