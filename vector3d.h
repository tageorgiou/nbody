#ifndef VECTOR3D_H
#define VECTOR3D_H
class Vector3D {
	public:
		Vector3D(double x0 = 0.0, double y0 = 0.0, double z0 = 0.0);
		double x, y, z;
		inline double mag() const;
		inline double mag_sq() const;
		void zero();
		inline Vector3D operator+(const Vector3D &right) const;
		inline Vector3D operator-(const Vector3D &right) const;
		inline Vector3D& operator+=(const Vector3D &right);
		inline Vector3D operator/(const double &right) const;
		inline Vector3D& operator/=(const double &right);
		inline Vector3D operator*(const double &right) const;
		inline Vector3D& operator*=(const double &right);
		double operator*(const Vector3D &right) const;
		double& operator[](const int index);
};
#endif
