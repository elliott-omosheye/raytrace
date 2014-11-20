#ifndef _LITSCENE
#define _LITSCENE

//Same as a scene but with lights

#include "scene.h"
#include "light.h"

class LitScene : public Scene{
	int NLights; //number of lights;
	Light *Lighting;
	Colour Ambient;
	char* fileName;

public:
	LitScene() : Scene(), Ambient(0.2f,0.2f,0.2f) {NLights = 0;}
	
	void setLighting(int n) {NLights=n; Lighting = new Light[n];}

	void setFile(char*);
	
	void setAmbient(Colour colour) {Ambient = colour;}
	
	Light& lightAt(int i) {return Lighting[i];}
	
	int& numberOfLights() {return NLights;}
	
	Colour colourOnObject(GObject*,Point, Point);
	bool intersect(GObject*,Ray,Colour&,int);
		
	friend ostream& operator<<(ostream&,LitScene);  //writing
	friend istream& operator>>(istream&,LitScene&); //reading
};
	
#endif