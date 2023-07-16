#ifndef VECTOR3D_HPP_INCLUDED
#define VECTOR3D_HPP_INCLUDED

#include "vector2d.hpp"

class Vector3D : public Vector2D
{
public:
	Vector3D() = default;
	Vector3D(const Vector3D&) = default;
	Vector3D(const float, const float, const float);
	~Vector3D() = default;
public:
	static Vector3D Cross(const Vector3D& v1, const Vector3D& v2);
	static Vector3D ToAngle(const Vector3D& v);
	static bool IsZero(const Vector3D& v);
public:
	float getComponentZ() const;
	void setComponentZ(const float);
public:
	void operator+=(const Vector3D&);
	void operator-=(const Vector3D&);
	void operator*=(const float);
	Vector3D operator+(const Vector3D&);
	Vector3D operator-(const Vector3D&);
	Vector3D operator*(const float);

	friend std::ostream& operator<<(std::ostream&, const Vector3D&);


private:
	float m_z{0.0f};
};

#endif 