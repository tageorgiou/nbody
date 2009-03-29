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

double Vector3D::mag() const
{
	return sqrt(x*x+y*y+z*z);
}

double Vector3D::mag_sq() const
{
	return x*x+y*y+z*z;
}

void Vector3D::zero()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Vector3D Vector3D::operator+(const Vector3D &right) const
{
	Vector3D result(x+right.x, y+right.y, z+right.z);
	return result;
}

Vector3D Vector3D::operator-(const Vector3D &right) const
{
	Vector3D result(x-right.x, y-right.y, z-right.z);
	return result;
}

Vector3D& Vector3D::operator+=(const Vector3D &right)
{
	this->x += right.x;
	this->y += right.y;
	this->z += right.z;
	return *this;
}

double Vector3D::operator*(const Vector3D &right) const
{
	double dot = 0;
	dot += x*right.x;
	dot += y*right.y;
	dot += z*right.z;
	return dot;
}

Vector3D Vector3D::operator*(const double &right) const
{
	Vector3D result(x*right, y*right, z*right);
	return result;
}

Vector3D& Vector3D::operator*=(const double &right)
{
	this->x *= right;
	this->y *= right;
	this->z *= right;
	return *this;
}

Vector3D Vector3D::operator/(const double &right) const
{
	Vector3D result(x/right, y/right, z/right);
	return result;
}

Vector3D& Vector3D::operator/=(const double &right)
{
	this->x /= right;
	this->y /= right;
	this->z /= right;
	return *this;
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

