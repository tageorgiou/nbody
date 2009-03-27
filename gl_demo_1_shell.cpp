// 
// Torbert, 3.12.2009
// 
// OpenGL Demo, 3-D Example
// 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <sys/time.h>
#include "vector3d.h"
#include <unistd.h>
#include <vector>
using namespace std;

#define TARGETFPS 60

double time = 0.0;
///////////////////////////////
double pi=3.1415926;
const double dt = 1e-2;
const double G = 1.0e-1;
const double ema_a = 0.1;
bool firststep = true;


double ema_e = 0.0;
double ema_fps = 60.0;
double prev_energy = 0.0;

int	w=640,h=480;
GLUquadric *background;

double rho,phi,theta,up=1.0,oldtheta,oldphi,counter=0.0;
double xc,yc,zc,xe,ye,ze;
int mousebutton,clickedx,clickedy;
struct timeval timea;
struct timeval timeb;

typedef struct {
	double mass;
	double x;
	double y;
	double z;
	double coord[3];
	double radius;
} body;

class Body {
	public:
		Body(double massin = 1.0, double rad = 0.1);
		double mass;
		Vector3D position;
		Vector3D prevposition;
		Vector3D velocity;
		Vector3D prevvelocity;
		Vector3D accel;
		Vector3D prevaccel;
		double heatenergy;
		double x() {return position[0];};
		double y() {return position[1];};
		double z() {return position[2];};
		double size;
		void simulate(double dt);
		void merge(Body &b);
};

Body::Body(double massin, double rad) {
	heatenergy = 0.0;
	mass = massin;
	size = rad;
}

void Body::merge(Body &b) {
	double c1 = mass/(b.mass+mass);
	double c2 = b.mass/(b.mass+mass);
	double ke_1 = (mass*prevvelocity.mag_sq() + b.mass*prevvelocity.mag_sq())/2;
//	heatenergy-=G*mass*b.mass/(position-b.position).mag();
	position = position*c1 + b.position*c2;
	prevposition = prevposition*c1 + b.prevposition*c2;
	velocity = velocity*c1 + b.velocity*c2;
	prevvelocity = prevvelocity*c1 + b.prevvelocity*c2;
	accel = accel*c1 + b.accel*c2;
	prevaccel = prevaccel*c1 + b.prevaccel*c2;
	mass = mass + b.mass;
	size = pow(b.size*b.size*b.size + size*size*size,1.0/3.0);
	heatenergy = heatenergy + b.heatenergy;
	printf("collision!\n");	
	double ke_2 = mass*prevvelocity.mag_sq()/2;
	heatenergy += ke_1 - ke_2;
}

void Body::simulate(double dt) {
//	printf("a:%f %f %f\n",accel[0],accel[1],accel[2]);
//	printf("v:%f %f %f\n",velocity[0],velocity[1],velocity[2]);
//	printf("r:%f %f %f\n",position[0],position[1],position[2]);

	//finish previous timestep!!
	velocity += accel*dt/2;

	prevposition = position;
	prevvelocity = velocity;	

	position += velocity*dt;
	position += accel*dt*dt/2;

	velocity += accel*dt/2;
	//timestep is unfinished!
	accel.zero();

//	prevposition = position;
//	prevvelocity = velocity;
//	if (firststep)
//		prevaccel = accel;
//	position = prevposition + prevvelocity * dt + prevaccel * dt * dt / 2;
//	velocity = prevvelocity + prevaccel * dt / 2 + accel * dt / 2;
//	prevaccel = accel;
//	accel.zero();
}

//Body bodies[16];
vector<Body> bodies;
///////////////////////////////

double systemEnergy();
void interact(Body &a, Body &b);
void drawString(char* s);
void display(void);
void look();
void idle(void);
void step();
void mouse(int button,int state,int xscr,int yscr);
void motion(int xscr,int yscr);
void mouse_wheel(int wheel,int direction,int xscr,int yscr);
void keyfunc(unsigned char key,int xscr,int yscr);
void reshape(int wscr,int hscr);
void lighting();
///////////////////////////////

void lighting() {
	GLfloat global_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specularLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat position[] = { -0.0f, 0.0f, -0.0f, 1.0f };
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
}

double systemEnergy()
{
	double energy = 0;
	//potentials
	for (int n = 0; n < bodies.size(); n++) {
		for (int m = n + 1; m < bodies.size(); m++) {
			energy-=G*bodies[n].mass*bodies[m].mass/(bodies[n].prevposition-bodies[m].prevposition).mag();
		}
	}
	//kinetic energy
	double potential = energy;
	for (int n = 0; n < bodies.size(); n++) {
		energy+=bodies[n].mass*bodies[n].prevvelocity.mag_sq()/2;
		energy+=bodies[n].heatenergy;
	}
	return energy;
}

void interact(Body &a, Body &b)
{
	double distance_sq = (a.position-b.position).mag_sq();
	double mag = G * a.mass * b.mass / distance_sq;
	//a.accel+=(b.position-a.position)*mag/sqrt(distance_sq)/a.mass;
	b.accel+=(a.position-b.position)*mag/sqrt(distance_sq)/b.mass;
}

void drawString(char* s)
{
	int k;
	for(k=0;k<strlen(s);k++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,s[k]);
}

void display(void)
{
	double t;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
//	glColor3f(cos(2*counter),sin(3*counter+M_PI/8),sin(coujnter));
	float mblack[] = {0.0f,0.0f,0.0f,1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mblack);

	glDepthMask(GL_FALSE);
	gluSphere(background,400,16,16);
	glDepthMask(GL_TRUE);
	
	lighting();

	//glPushAttrib(GL_NORMALIZE);
	//glFrontFace(GL_CW);
//	glColor3f(0.8,0.8,0.1);
//	glutSolidTeapot(0.15);
	//glPushAttrib(GL_NORMALIZE);

	//glColor3f(0.0,0.2,0.7);
	float mcolor[] = {0.0f,0.0f,0.65f,1.0f};
	float specReflect[] = {0.7f,0.7f,0.7f,1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflect);
	glMateriali(GL_FRONT, GL_SHININESS, 96);
//	printf("--\n");
	for (int i = 0; i < bodies.size(); i++) {
		glPushMatrix();
		//printf("%f\n",bodies[i].heatenergy);
		float mcolor2[] = {log(bodies[i].heatenergy)/100,0.0f,0.65f,1.0f};
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor2);
		glTranslatef(bodies[i].x(),bodies[i].y(),bodies[i].z());
		glutSolidSphere(bodies[i].size,32,32);
		glPopMatrix();
	}

	//FPS code
	timea=timeb;
	gettimeofday(&timeb,NULL);
	double us = (timeb.tv_sec-timea.tv_sec)*1000000 + (timeb.tv_usec-timea.tv_usec);
//	if (1.0e6/TARGETFPS > us)
		//usleep(1.0e6/TARGETFPS - us);
	char str[80];
	ema_fps = 1e6/us*ema_a + (1-ema_a)*ema_fps;
	sprintf(str,"%3.1f",ema_fps);
	glColor3f(1.0,1.0,1.0);
	glRasterPos3f(0.0,0.1,0.0);
	drawString(str);
	glutSwapBuffers();
	printf("energy:%f t:%f delta E: %F\n",systemEnergy(),time,ema_e);
	//printf("%f %f\n",time,systemEnergy());
}
void look()
{
	if (theta > M_PI*2)
		theta-=2*M_PI;
	if (theta < 0)
		theta+=2*M_PI;
	if (theta > M_PI)
		up = -1.0;
	else
		up = 1.0;
	xe=xc+rho*sin(theta)*sin(phi); // y
	ye=yc+rho*cos(theta);		  // z
	ze=zc+rho*sin(theta)*cos(phi); // x
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xe,ye,ze, xc,yc,zc, 0.0,up,0.0);
}
void step() {
	time+=dt;
	counter+=0.01;
	for (int n = 0; n < bodies.size(); n++) {
		#pragma omp parallel for
		for (int m = 0; m < bodies.size(); m++) {
			if (n==m)
				continue;
			interact(bodies[n],bodies[m]);
		}
	}
	#pragma omp parallel for
	for (int n = 0; n < bodies.size(); n++) {
		bodies[n].simulate(dt);
	}

	int n = 0;
	while (n < bodies.size()) {
		int m = n+1;
		while (m < bodies.size()) {
			if ((bodies[n].position-bodies[m].position).mag() < bodies[n].size + bodies[m].size) {
				bodies[n].merge(bodies[m]);
				bodies.erase(bodies.begin() + m);
			} else {
				m++;
			}
		}
		n++;
	}


	double energy = systemEnergy();
	ema_e = (energy-prev_energy)*ema_a+(1-ema_a)*ema_e;
	prev_energy = energy;
	firststep = false;
}
void idle(void)
{
	for (int n = 0; n < 1; n++)
		step();
	look();
	glutPostRedisplay();
}
void mouse(int button,int state,int xscr,int yscr)
{
	mousebutton = button;
	clickedx    = xscr;
	clickedy    = yscr;

	oldtheta = theta;
	oldphi = phi;
}
void motion(int xscr,int yscr)
{
	phi = (double)(xscr-clickedx)/200+oldphi;
	theta = (double)(yscr-clickedy)/200+oldtheta;
	look();
}
void mouse_wheel(int wheel,int direction,int xscr,int yscr)
{
	printf("%d\n",direction);
	if (direction < 0) {
		rho*=0.9;
	} else {
		rho*=1.1;
	}
	look();

}
void keyfunc(unsigned char key,int xscr,int yscr)
{
	if(key=='q')
	{
		exit(0);
	}
}
void reshape(int wscr,int hscr)
{
	GLfloat aspect_ratio;

	w=wscr;
	h=hscr;
	aspect_ratio=(GLfloat)w/(GLfloat)h;
	glViewport(0,0,(GLsizei)w,(GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,aspect_ratio,0.1,500.0);

	look();
}
void init2body()
{
	for (int i = 0; i < 2; i++)
		bodies.push_back(Body());
	bodies[0].mass = 50.0;
	bodies[1].mass = 50.0;
	bodies[0].position[2] = -0.5;
	bodies[1].position[2] = 0.5;
	bodies[0].velocity[0] = 1.5;
	bodies[1].velocity[0] = -1.5;
}
void init3body()
{
	for (int i = 0; i < 3; i++)
		bodies.push_back(Body());
	bodies[0].mass = 50.0;
	bodies[1].mass = 50.0;
	bodies[0].position[2] = -0.5;
	bodies[1].position[2] = 0.5;
	bodies[0].velocity[0] = 1.5;
	bodies[1].velocity[0] = -1.5;
	bodies[2].mass = 10.0;
	bodies[2].size = 0.08;
}
void init8body()
{
	int nbodies = 16;
	for (int n = 0; n < nbodies; n++) {
		bodies.push_back(Body());
		bodies[n].position[0] = (2+n%2*1.7)*cos(2*M_PI*n/nbodies);
		bodies[n].position[1] = (2+n%2*1.7)*sin(2*M_PI*n/nbodies);
		bodies[n].velocity[0] = -(2+n%2*1.7)*sin(2*M_PI*n/nbodies);
		bodies[n].velocity[1] = (2+n%2*1.7)*cos(2*M_PI*n/nbodies);
		bodies[n].mass = 50.0;
	}
}
void initlotsbodies()
{
	for (int n = 0; n < 500; n++) {
		bodies.push_back(Body());
		bodies[n].position[0] = (double)rand()/RAND_MAX*20-10;
		bodies[n].position[1] = (double)rand()/RAND_MAX*20-10;
		bodies[n].position[2] = (double)rand()/RAND_MAX*20-10;
		bodies[n].velocity[0] = (double)rand()/RAND_MAX*10-5;
		bodies[n].velocity[1] = (double)rand()/RAND_MAX*10-5;
		bodies[n].velocity[2] = (double)rand()/RAND_MAX*10-5;
		bodies[n].mass = (double)rand()/RAND_MAX*50;
		bodies[n].size = pow(bodies[n].mass,1.0/3.0)/10;
	}
}
int main(int argc,char* argv[])
{  
	initlotsbodies();
	rho=3.1;
	phi=0.0;
	theta=pi/2.0;
	xc=yc=zc=0.0;

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(w,h);
	glutInitWindowPosition(100,50);
	glutCreateWindow("OpenGL Demo");

	glClearColor(1.0,1.0,1.0,0.0);
	glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glFrontFace(GL_CCW);

	background = gluNewQuadric();
	gluQuadricTexture(background, GL_TRUE);

	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mouse_wheel);
	glutKeyboardFunc(keyfunc);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}
