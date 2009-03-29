#ifndef VECTOR3D_H
#define VECTOR3D_H
class Vector3D {
	public:
		Vector3D(double x0 = 0.0, double y0 = 0.0, double z0 = 0.0);
		double x, y, z;
		double mag();
		double mag_sq();
		void zero();
		Vector3D operator+(const Vector3D &right);
		Vector3D operator-(const Vector3D &right);
		Vector3D& operator+=(const Vector3D &right);
		Vector3D operator/(const double &right);
		Vector3D& operator/=(const double &right);
		Vector3D operator*(const double &right);
		Vector3D& operator*=(const double &right);
		double operator*(const Vector3D &right);
		double& operator[](const int index);
};
#endif
