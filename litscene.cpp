#include "litscene.h"
#include <math.h>

ostream& operator<<(ostream& s, LitScene scene)
{
	scene.print(s);
	
	s << "\n\nLighting:\n";
	
	int i;
	s << "Ambient: " << scene.Ambient <<"\n";
	for(i=0;i<scene.numberOfLights();++i) s << "Light: " << i << " " << scene.lightAt(i) << "\n";
	
	return s;
}
	



istream& operator>>(istream& s, LitScene& scene)
//reading
{
	scene.read(s);
	//read the ambient lighting must always be there
	s >> scene.Ambient;
	
	//read how many lights
	int n; s >> n;
	
	if(n==0) return s;
	
	scene.setLighting(n);
	
	Light light; int i;		
	for(i=0;i<n;++i) {
		s >> light;
		scene.lightAt(i) = light;
	}
	return s;
}

void LitScene::setFile(char* file) {
	fileName = file;
}

static float power(float x, int n)
{
    if (n < 1) return 1.0;
    if (n == 1) return x;
    else return x * power(x, n - 1);
}

Colour LitScene::colourOnObject(GObject *object, Point p, Point eye)
{
    bool shadow = false;
	Vector  n = object->normal(p); //normal vector
    Ray shadowray;
	
	Colour colour;
	

	//compute ambient component
	Colour ka = object->material().ambient();
	colour = ka*Ambient;
	

	for(int i=0; i<NLights; ++i){
		Light light = lightAt(i);
	    // Find the direction to the light from the point 
		
		Vector vL(0,0,0);
		if(light.directional()) {
			Vector tmpVector = light.vector();
			vL = vminus(vL,tmpVector);
			vL = vL.normalised();
		}
		else { 
			Point tmpPoint = light.point();
			vL = (tmpPoint - p).normalised();
		}

		/*Is the light occluded by another object?*/
		Ray reverse;
		shadowray.origin() = light.point();//create ray from light to object point
		shadowray.direction() = (p-light.point()).normalised();			
		//variables for next part
		bool found = false;
		float r; //distance from point to intersecting object
		for(int iter = 0; iter < numObjects(); ++iter){ //for each object
			float t; //distance from light to intersecting object
			Colour col;
			//check if both rays intersect with any onjects
			if((at(iter) != object) && at(iter)->intersect(shadowray,t,col) && (t > 0.0)){
				//intersection found
				reverse.direction() = vL; //create ray from object point to light
				reverse.origin() = p;
				if (at(iter)->intersect(reverse,r,col) && (r>0)) //second ray
					found = true;
			} 
		}
		//...
		

		// does the light illuminate this point? 		
		float nl = n^vL; //dot product
		float nh;
		Vector e = eye-p;
		e.normalise();
		Vector h = vadd(e,vL);
		h.normalise(); // (V + L) / |L + V|)
		nh = n^h;
      
		if(nl > 0.0){
			
	    // if so add diffuse and specular component 
			
			Colour kd = object->material().diffuse();
			Colour Diffuse = (kd * nl);	
			Colour ks = object->material().specular();	
			Colour Specular = (ks * power(nh, (int) object->material().shininess()));
			Colour AddedColour = (Diffuse+Specular);	
			Colour Intensity = light.intensity();
			Colour Reflection = Intensity*AddedColour;
			if (found) Reflection = Reflection*0.0f; //if in shadow remove lighting
			colour = colour + Reflection;
		}
	}
	
	//check that the colour is in the bounds 0 to 1
	colour.check();
	
	return colour;
}
	
	
	
	
bool LitScene::intersect(GObject* me, Ray ray, Colour& colour, int depth)
/*returns true if the ray intersects the scene, and if so then the colour
at the intersection point. This overrides the method in Scene*/
{
	double tmin = 9999999999.9;
	bool found = false;
	GObject* object = NULL;
    Ray reflected;
    if (depth == 0) return false; /* end of recursion */


	//for each object
	for(int i = 0; i < numObjects(); ++i){
		float t;
		Colour col;
		if((at(i) != me) && at(i)->intersect(ray,t,col)){
			//intersection found
			if((t < tmin) && (t > 0.0) ){
				object = at(i);
				tmin = t;
				colour = col;
				found = true;
			}
		} 
	}
	if(found){//an object has been found with the smallest intersection
        Colour refl_col;
		/*find the intersection point*/
		Point p = ray.pointAt((float)tmin);
		Vector I, n = object->normal(p).normalised(); //normal vector
		
		//now we want the colour computed at point p on the object
		colour = colourOnObject(object, p, ray.origin());
		
		/*Recursive ray trace. Use specular
           colour to modulate the colour returned by the recursive
           intersect call */
			I = vadd((p-ray.origin()),n*(n^(ray.origin()-p))*2);//reflecting vector
			reflected.direction() = I.normalised();
			reflected.origin() = p;
			if (intersect(object,reflected,refl_col,depth-1))
			colour = colour + object->material().specular()*refl_col*(float(1/(tmin*2)));

			//refractions
			if(true){
				Ray T;
				float IoR = (1.0f/1.5f); //index of refraction for glass
				float cosA = ray.direction()^n;
				T.direction() = vadd((ray.direction()*(IoR)),n*(IoR*(cosA)-sqrt(1+IoR*(power(cosA,2)-1)))).normalised();
				T.origin()=p;
				if (intersect(object,T,refl_col,depth-1))
				colour = colour + refl_col * object->material().ambient(); //would normally use transparency
				
			}
		// ...

		//check that the colour is in the bounds 0 to 1
        colour.check();  
		return true;
	}
	else 
	{
		return false;
	}
}
