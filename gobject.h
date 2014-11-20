#ifndef _GOBJECT
#define _GOBJECT

#include "colour.h"
#include "material.h"
#include "ray.h"
#include <iostream>

using namespace std;

//an abstract class
class GObject{
	Material Mat;	//the material reflectance properties of the object
public:
	GObject() : Mat() {};
	
	Material& material() {return Mat;}	
	
	virtual bool intersect(Ray ray, float& t, Colour& colour) = 0;
	virtual Vector normal(Point) = 0;
	
	virtual void print(ostream&) = 0;
	
	//for each different kind of object need one of these
	virtual void read(istream&) = 0;
};

#endif