CC= c++
GLUT_HOME = /usr/opt/glut/build/glut-3.6

#############################################################################
# compiling #################################################################
CFLAGS = -I${GLUT_HOME}/include

.cpp.o:
	${CC} ${CFLAGS} -c $< -o $@

#############################################################################
# linking #################################################################
LIBS = -lglut -lGL -lGLU -lX11 -lXmu -lXi
LIBDIR = -L${GLUT_HOME}/lib/glut -L/usr/X11R6/lib -L/usr/lib 

all: ray testit 

colour.o : colour.cpp
	${CC} ${CFLAGS} -c $? -o $@
light.o :  light.cpp
	${CC} ${CFLAGS} -c $? -o $@
litscene.o : litscene.cpp
	${CC} ${CFLAGS} -c $? -o $@
material.o : material.cpp
	${CC} ${CFLAGS} -c $? -o $@
point.o : point.cpp
	${CC} ${CFLAGS} -c $? -o $@
polygon.o : polygon.cpp
	${CC} ${CFLAGS} -c $? -o $@
ray.o : ray.cpp
	${CC} ${CFLAGS} -c $? -o $@
scene.o : scene.cpp
	${CC} ${CFLAGS} -c $? -o $@
simplecamera.o : simplecamera.cpp
	${CC} ${CFLAGS} -c $? -o $@
sphere.o : sphere.cpp
	${CC} ${CFLAGS} -c $? -o $@
vector.o : vector.cpp
	${CC} ${CFLAGS} -c $? -o $@
mainray.o : mainray.cpp
	${CC} ${CFLAGS} -c $? -o $@
main.o : main.cpp
	${CC} ${CFLAGS} -c $? -o $@



objects = colour.o material.o point.o ray.o  sphere.o polygon.o \
          vector.o scene.o simplecamera.o light.o litscene.o

ray: 	$(objects) mainray.o
	$(CC) ${LIBDIR} ${LIBS} $(objects) mainray.o -lglut -lGLU -lGL -lXmu  -lX11 -lm -o ray

testit: $(objects) main.o
	$(CC) ${LIBDIR} ${LIBS} $(objects) main.o -lm -o testit

clean:
	\rm -f *.o ray testit

