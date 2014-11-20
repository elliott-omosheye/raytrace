#ifndef _MATERIAL
#define _MATERIAL

#include "colour.h"

class Material{
	Colour Ambient;
	Colour Diffuse;
	Colour Specular;
	float Shininess;
public :
	Material() : Ambient(0.0,0.0,0.0), Diffuse(0.0,0.0,0.0), 
		     Specular(0.0,0.0,0.0), Shininess(0.0) {};
	Material(const Material&);	  		//initialisation 
	Material& operator=(const Material&); 		//assignment
	
	
	Colour& ambient() {return Ambient;}
	Colour& diffuse() {return Diffuse;}
	Colour& specular() {return Specular;}
	float& shininess() {return Shininess;}
	
	friend ostream& operator<<(ostream&,Material);  //writing
	friend istream& operator>>(istream&,Material&); //reading
};
	
#endif