all: nbody nbody_serial
nbody: gl_demo_1_shell.cpp vector3d.h nbody.cpp
	g++ -g -fopenmp -o nbody -O2 -march=core2 -lm -L/usr/lib -lGLU -lGL -lglut -L/usr/X11R6/lib -lX11 -lXext -lXi -lXmu nbody.cpp
clean: 
	rm nbody nbody_serial
nbody_serial: gl_demo_1_shell.cpp nbody.cpp vector3d.h
	g++ -g -o nbody_serial -O2 -march=core2 -lm -L/usr/lib -lGLU -lGL -lglut -L/usr/X11R6/lib -lX11 -lXext -lXi -lXmu nbody.cpp
