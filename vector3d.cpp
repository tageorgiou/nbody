#include "vector3d.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Vector3D::Vector3D(double x0, double y0, double z0)
{
	x = x0;
	y = y0;
	z = z0;
}

double Vector3D::mag()
{
	return sqrt(x*x+y*y+z*z);
}

double Vector3D::mag_sq()
{
	return x*x+y*y+z*z;
}

void Vector3D::zero()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Vector3D Vector3D::operator+(const Vector3D &right)
{
	Vector3D result(x+right.x, y+right.y, z+right.z);
	return result;
}

Vector3D Vector3D::operator-(const Vector3D &right)
{
	Vector3D result(x-right.x, y-right.y, z-right.z);
	return result;
}

Vector3D& Vector3D::operator+=(const Vector3D &right)
{
	#pragma omp atomic
	this->x += right.x;
	#pragma omp atomic
	this->y += right.y;
	#pragma omp atomic
	this->z += right.z;
}

double Vector3D::operator*(const Vector3D &right)
{
	double dot = 0;
	dot += x*right.x;
	dot += y*right.y;
	dot += z*right.z;
	return dot;
}

Vector3D Vector3D::operator*(const double &right)
{
	Vector3D result(x*right, y*right, z*right);
	return result;
}

Vector3D& Vector3D::operator*=(const double &right)
{
	#pragma omp atomic
	this->x *= right;
	#pragma omp atomic
	this->y *= right;
	#pragma omp atomic
	this->z *= right;
}

Vector3D Vector3D::operator/(const double &right)
{
	Vector3D result(x/right, y/right, z/right);
	return result;
}

Vector3D& Vector3D::operator/=(const double &right)
{
	#pragma omp atomic
	this->x /= right;
	#pragma omp atomic
	this->y /= right;
	#pragma omp atomic
	this->z /= right;
}

double& Vector3D::operator[](const int index)
{
	switch (index) {
		case (0):
			return x;
		case (1):
			return y;
		case (2):
			return z; 
	}
}

