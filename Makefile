all: gl_demo_1_shell
gl_demo_1_shell: gl_demo_1_shell.cpp vector3d.cpp vector3d.h
	g++ -o gl_demo_1_shell -O2 -march=core2 -lm -L/usr/lib -lGLU -lGL -lglut -L/usr/X11R6/lib -lX11 -lXext -lXi -lXmu gl_demo_1_shell.cpp vector3d.cpp
clean: 
	rm gl_demo_1_shell
