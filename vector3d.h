#ifndef VECTOR3D_H
#define VECTOR3D_H
#include <math.h>
class Vector3D {
	public:
		Vector3D(double x0 = 0.0, double y0 = 0.0, double z0 = 0.0)
		{
			x = x0;
			y = y0;
			z = z0;
		}
		double x, y, z;
		inline double mag() const {	return sqrt(x*x+y*y+z*z);};
		inline double mag_sq() const {return x*x+y*y+z*z;};
		void zero() { x = y = z = 0.0; };
		inline Vector3D operator+(const Vector3D &right) const {return Vector3D(x+right.x, y+right.y, z+right.z);};
		inline Vector3D operator-(const Vector3D &right) const {return Vector3D(x-right.x, y-right.y, z-right.z);};
		inline Vector3D& operator+=(const Vector3D &right)
		{
			this->x += right.x;
			this->y += right.y;
			this->z += right.z;
			return *this;
		};
		inline Vector3D operator/(const double &right) const {return Vector3D(x/right, y/right, z/right);};
		inline Vector3D& operator/=(const double &right)
		{
			this->x /= right;
			this->y /= right;
			this->z /= right;
			return *this;
		};
		inline Vector3D operator*(const double &right) const {return Vector3D(x*right, y*right, z*right);};
		inline Vector3D& operator*=(const double &right)
		{
			this->x *= right;
			this->y *= right;
			this->z *= right;
			return *this;
		};
		double operator*(const Vector3D &right) const
		{
			double dot = 0;
			dot += x*right.x;
			dot += y*right.y;
			dot += z*right.z;
			return dot;
		};
		double& operator[](const int index)
		{
			switch (index) {
				case (0):
					return x;
				case (1):
					return y;
				case (2):
					return z; 
			}
		};
};
#endif
