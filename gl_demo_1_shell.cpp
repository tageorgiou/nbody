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

///////////////////////////////
double pi=3.1415926;
const double dt = 5e-3;
const double G = 5.0e-3;

int	w=640,h=480;

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
		Body(double massin = 1.0);
		double mass;
		double position[3];
		double velocity[3];
		double accel[3];
		double x() {return position[0];};
		double y() {return position[1];};
		double z() {return position[2];};
		double accelx(double a) {accel[0]+=a;};
		double accely(double a) {accel[1]+=a;};
		double accelz(double a) {accel[2]+=a;};
		void simulate(double dt);
};

Body::Body(double massin) {
	mass = massin;
}

void Body::simulate(double dt) {
	printf("a:%f %f %f\n",accel[0],accel[1],accel[2]);
	printf("v:%f %f %f\n",velocity[0],velocity[1],velocity[2]);
	printf("r:%f %f %f\n",position[0],position[1],position[2]);
	for (int i = 0; i < 3; i++) {
		position[i] += velocity[i]*dt;
		position[i] += accel[i]*dt*dt/2;
		velocity[i] += accel[i]*dt;
		accel[i] = 0.0;
	}
}

Body bodies[3];
///////////////////////////////

double magnitude_sq(const double* a);
double systemEnergy();
void interact(Body &a, Body &b);
void drawString(char* s);
void display(void);
void look();
void idle(void);
void mouse(int button,int state,int xscr,int yscr);
void motion(int xscr,int yscr);
void mouse_wheel(int wheel,int direction,int xscr,int yscr);
void keyfunc(unsigned char key,int xscr,int yscr);
void reshape(int wscr,int hscr);

///////////////////////////////

double magnitude_sq(const double* a)
{
	return a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
}

double systemEnergy()
{
	double energy = 0;
	//potentials
	energy-=G*bodies[0].mass*bodies[1].mass/sqrt((bodies[0].x()-bodies[1].x())*(bodies[0].x()-bodies[1].x())+(bodies[0].y()-bodies[1].y())*(bodies[0].y()-bodies[1].y())+(bodies[0].z()-bodies[1].z())*(bodies[0].z()-bodies[1].z()));
	energy+=bodies[0].mass*magnitude_sq(bodies[0].velocity)/2;
	energy+=bodies[1].mass*magnitude_sq(bodies[1].velocity)/2;
	return energy;
}

void interact(Body &a, Body &b)
{
	double distance_sq = (a.x()-b.x())*(a.x()-b.x())+(a.y()-b.y())*(a.y()-b.y())+(a.z()-b.z())*(a.z()-b.z());
	double mag = G * a.mass * b.mass / distance_sq;
	printf("mag:%f\n",mag);
	a.accelx((b.x()-a.x())*mag/sqrt(distance_sq));
	a.accely((b.y()-a.y())*mag/sqrt(distance_sq));
	a.accelz((b.z()-a.z())*mag/sqrt(distance_sq));
	b.accelx((a.x()-b.x())*mag/sqrt(distance_sq));
	b.accely((a.y()-b.y())*mag/sqrt(distance_sq));
	b.accelz((a.z()-b.z())*mag/sqrt(distance_sq));
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

	glClear(GL_COLOR_BUFFER_BIT); // clear the screen
	glColor3f(cos(2*counter),sin(3*counter+M_PI/8),sin(counter));

	glutWireTeapot(0.5);

	glPushMatrix();
	glColor3f(0.0,0.0,0.0);
	glTranslatef(1.0,0.0,0.0);
	glutWireSphere(0.2,6,6);
	glPopMatrix();

	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(bodies[i].x(),bodies[i].y(),bodies[i].z());
		glutWireSphere(0.1,8,8);
		glPopMatrix();
	}

	//FPS code
	timea=timeb;
	gettimeofday(&timeb,NULL);
	double us = (timeb.tv_sec-timea.tv_sec)*1000000 + (timeb.tv_usec-timea.tv_usec);
	char str[80];
	sprintf(str,"%3.1f",1e6/us);
	glColor3f(0.0,0.0,1.0);
	glRasterPos2i(0,0);
	drawString(str);
	glutSwapBuffers();
	printf("energy:%f\n",systemEnergy());
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
void idle(void)
{
	counter+=0.01;
	if(up>0.0)
	{
		if(theta+0.001>pi)
		{
			up*=-1.0;
			theta=-pi;
		}
	}
	else if(theta+0.001>0.0)
			up*=-1.0;
	//theta+=0.01;

	//phi+=0.01;
	//rho+=sin(2*phi)*0.03 + cos(theta)*0.01;
//	rho+=0.005;
	interact(bodies[0],bodies[1]);
	bodies[0].simulate(dt);
	bodies[1].simulate(dt);
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

	theta      +=0.1;
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

	bodies[0].mass = 50.0;
	bodies[1].mass = 50.0;
	bodies[0].position[2] = -1.0;
	bodies[1].position[2] = 1.0;
	bodies[0].velocity[0] = 1.5;
	bodies[1].velocity[0] = -1.5;


}
int main(int argc,char* argv[])
{  
	init2body();
	rho=3.1;
	phi=0.0;
	theta=pi/2.0;
	xc=yc=zc=0.0;

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(w,h);
	glutInitWindowPosition(100,50);
	glutCreateWindow("OpenGL Demo");

	glClearColor(1.0,1.0,1.0,0.0);
	glShadeModel(GL_SMOOTH);

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
