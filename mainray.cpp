#include <stdlib.h>
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>

#include "litscene.h"
#include "simplecamera.h"
#include <omp.h>
#include <vector>
#include <windows.h>
#include <math.h>

using namespace std;

LitScene TheScene;
SimpleCamera TheCamera(250,250);
SimpleCamera SecondCamera(250,250);
int Nx, Ny;//resolution

#define DEPTH 4

void set_pixel(int x, int y, Colour col)
{
    	glBegin(GL_POINTS);
        	glColor3f(col.red(), col.green(), col.blue());
        	glVertex2i(x, y);
    	glEnd();
}

void display(void)
{
	DWORD start = GetTickCount();
	vector<int> xCoor;
	vector<int> yCoor;
	vector<Colour> val;
	int SSAA = 2; //Anti-Aliasing value - set at 1 does nothing
    	int i, j;
    	Colour col;
	Colour colTwo;
	Colour result;
	TheCamera.setResolution(Nx*SSAA,Ny*SSAA);
	SecondCamera.setResolution(Nx*SSAA,Ny*SSAA);
        /* clear the window, set it to the background colour */
	#pragma omp parallel for firstprivate(j) private(col,colTwo,result) //work-share split up job to threads
	//Super Sampling Anti-Aliasing
	for(i=0;i<SSAA*Nx;i=i+SSAA){										    
		for(j=0;j<SSAA*Ny;j=j+SSAA){
			for (int AAx=-SSAA+1;AAx<SSAA;AAx++){
				for (int AAy=-SSAA+1;AAy<SSAA;AAy++){
					Ray ray = TheCamera.ray(i+AAx,j+AAy);
					Ray	rayTwo = SecondCamera.ray(i+AAx,j+AAy);
					if (TheScene.intersect(NULL, ray, col, 1)&&(TheScene.intersect(NULL, rayTwo, colTwo, 1))){
						col.blue() = 0;
						col.green() = col.green() * (float)0.5;
						colTwo.red() = 0;
						colTwo.green() = colTwo.green()*(float)0.5;
						col = col+colTwo;
						col.check();
						result = result + col;
					}
				}
			}
			#pragma omp critical //only one thread at time 
				{
					//save results
					result = result*(1/pow(SSAA,3));
					result.check();
					xCoor.push_back((int)(i/SSAA));
					yCoor.push_back((int)(j/SSAA));
					val.push_back(result);
				}		
				result.reset(0,0,0);
		}
	}//leave multithreading
	for(int w=0;w<(int)xCoor.size();w++){
		set_pixel(xCoor[w],yCoor[w],val[w]); //read saved results and set pixel colour
	}
        /* force any unfinished OpenGL commands to be done */
    	glFlush();
    	DWORD end = GetTickCount();
	cout << "Finished\n" << (end-start) <<"\n";

}


void create_window(void)
{
	Nx = TheCamera.xResolution();
	Ny = TheCamera.yResolution();
	
    /* create a window with the desired size and title */   
	glutInitWindowSize(Nx, Ny);
	glutCreateWindow("simple graphics");

    /* specify the function to be used for rendering in this window */
	glutDisplayFunc(display);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluOrtho2D(0.0,(double)Nx-1,0.0,(double)Ny-1);
	glViewport(0,0,Nx,Ny);
}

/***************************************************************************/
/* The main function */
/***************************************************************************/
int main(int argc, char **argv)
{
	/* initialise the window manager library */
    	glutInit(&argc, argv);

        /* set up the camera */
      	TheCamera.setResolution(750,750);
    	TheCamera.setVPWindow(-2.0,2.0,-2.0,2.0);
    	TheCamera.zcop() = 2.0;
		SecondCamera.setResolution(500,500);
		 	SecondCamera.setVPWindow(-2.0,2.0,-2.0,2.0);
    	SecondCamera.zcop() = 2.0;
        /* read the scene from file*/

		const char* fileName;

		if (argc < 2) {
			fileName = "complexscene.dat";
		} else {
			fileName = argv[1];
		}		

		ifstream sceneFile (fileName);
		if (!sceneFile.is_open()) {
			cout << "Couldn't open file " << fileName << "!" << endl;
		}
		
		sceneFile >> TheScene;

		sceneFile.close();
    	
    	cout << TheScene << "\n";

        /* open a window of the specified size */
    	create_window();

        /* enter the event loop */    
    	glutMainLoop();
}
