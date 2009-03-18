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
double time = 0.0;
///////////////////////////////
double pi=3.1415926;
const double dt = 1e-4;
const double G = 1.0e-1;

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
//	printf("a:%f %f %f\n",accel[0],accel[1],accel[2]);
//	printf("v:%f %f %f\n",velocity[0],velocity[1],velocity[2]);
//	printf("r:%f %f %f\n",position[0],position[1],position[2]);
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
void lighting();
///////////////////////////////

void lighting() {
	GLfloat global_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.1f, 0.1f, 0.1, 1.0f };
	GLfloat specularLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat position[] = { -1.5f, 1.5f, -4.0f, 1.0f };
}

double magnitude_sq(const double* a)
{
	return a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
}

double systemEnergy()
{
	double energy = 0;
	//potentials
	energy-=G*bodies[0].mass*bodies[1].mass/sqrt((bodies[0].x()-bodies[1].x())*(bodies[0].x()-bodies[1].x())+(bodies[0].y()-bodies[1].y())*(bodies[0].y()-bodies[1].y())+(bodies[0].z()-bodies[1].z())*(bodies[0].z()-bodies[1].z()));
	//printf("p:%f\n",energy);
	double v1 = sqrt(magnitude_sq(bodies[0].velocity));
//	energy += bodies[0].mass*v1*v1/2;
//	printf("%f\n",energy);
//	printf("%f * %f * %f / 2\n", bodies[0].mass, v1, v1);
	energy+=bodies[0].mass*(bodies[0].velocity[0]*bodies[0].velocity[0]+bodies[0].velocity[1]*bodies[0].velocity[1]+bodies[0].velocity[2]*bodies[0].velocity[2])/2;
	energy+=bodies[1].mass*(bodies[1].velocity[0]*bodies[1].velocity[0]+bodies[1].velocity[1]*bodies[1].velocity[1]+bodies[1].velocity[2]*bodies[1].velocity[2])/2;
	//energy+=bodies[0].mass*magnitude_sq(bodies[0].velocity)/2;
	//energy+=bodies[1].mass*magnitude_sq(bodies[1].velocity)/2;
//	printf("v1: %f v2: %f\n",sqrt(magnitude_sq(bodies[0].velocity)),sqrt(magnitude_sq(bodies[1].velocity)));
	return energy;
}

void interact(Body &a, Body &b)
{
	double distance_sq = (a.x()-b.x())*(a.x()-b.x())+(a.y()-b.y())*(a.y()-b.y())+(a.z()-b.z())*(a.z()-b.z());
	double mag = G * a.mass * b.mass / distance_sq;
	printf("mag:%f\n",mag);
	a.accelx((b.x()-a.x())*mag/sqrt(distance_sq)/a.mass);
	a.accely((b.y()-a.y())*mag/sqrt(distance_sq)/a.mass);
	a.accelz((b.z()-a.z())*mag/sqrt(distance_sq)/a.mass);
	b.accelx((a.x()-b.x())*mag/sqrt(distance_sq)/b.mass);
	b.accely((a.y()-b.y())*mag/sqrt(distance_sq)/b.mass);
	b.accelz((a.z()-b.z())*mag/sqrt(distance_sq)/b.mass);
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
//	glColor3f(cos(2*counter),sin(3*counter+M_PI/8),sin(counter));

	glDepthMask(GL_FALSE);
	gluSphere(background,400,16,16);
	glDepthMask(GL_TRUE);
	
	//lighting();

	//glPushAttrib(GL_NORMALIZE);
	//glFrontFace(GL_CW);
	glColor3f(0.0,0.0,0.8);
	glutSolidTeapot(0.1);
	//glPushAttrib(GL_NORMALIZE);

	glColor3f(0.5,0.0,0.5);
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(bodies[i].x(),bodies[i].y(),bodies[i].z());
		glutSolidSphere(0.1,16,16);
		glPopMatrix();
	}

	//FPS code
	timea=timeb;
	gettimeofday(&timeb,NULL);
	double us = (timeb.tv_sec-timea.tv_sec)*1000000 + (timeb.tv_usec-timea.tv_usec);
	char str[80];
	sprintf(str,"%3.1f",1e6/us);
	glColor3f(1.0,1.0,1.0);
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
	time+=dt;
	printf("t:%f\n",time);
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
	printf("reshape\n");
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
	bodies[0].position[2] = -0.5;
	bodies[1].position[2] = 0.5;
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(w,h);
	glutInitWindowPosition(100,50);
	glutCreateWindow("OpenGL Demo");

	glClearColor(1.0,1.0,1.0,0.0);
	glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glFrontFace(GL_CCW);

	background = gluNewQuadric();
	gluQuadricTexture(background, GL_TRUE);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
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
