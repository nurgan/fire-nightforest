#pragma once
#ifndef _MULTISHAPE_H_
#define _MULTISHAPE_H_

#include <string>
#include <vector>
#include <memory>


class Program;
class Shape;
class Material;

class Multishape
{
public:
	Multishape();
	virtual ~Multishape();
	void loadMesh(const std::string &meshName);
   void loadMesh(const std::string &meshName, int min, int max);
	void init();
	void resize();
	void draw(const std::shared_ptr<Program> prog) const;
   void draw(const std::shared_ptr<Program> prog, std::vector<Material> materials) const;
   void draw(const std::shared_ptr<Program> prog, Material material) const;	

private:
	std::vector<Shape> shapes;
};

#endif
