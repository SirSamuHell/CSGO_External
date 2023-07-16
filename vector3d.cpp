#include "vector3d.hpp"
 
Vector3D::Vector3D(const float x, const float y, const float z) : Vector3D::Vector2D(x, y), m_z {z}
{
	
}

Vector3D Vector3D::Cross(const Vector3D& v1, const Vector3D& v2)
{
	return {
		v1.getComponentY() * v2.getComponentZ() - v1.getComponentZ() * v2.getComponentY(),
		v1.getComponentZ() * v2.getComponentX() - v1.getComponentX() * v2.getComponentZ(),
		v1.getComponentX() * v2.getComponentY() - v1.getComponentY() * v2.getComponentX()
	};
}

Vector3D Vector3D::ToAngle(const Vector3D& v)
{
	const auto x = std::atan2f(-v.getComponentZ(), std::hypotf(v.getComponentX(), v.getComponentY()) * (180.0f / 3.14f));
	const auto y = std::atan2f(v.getComponentY(), v.getComponentX()) * (180.0f / 3.14f);
	const auto z = 0.0f;
	return Vector3D{x, y, z};
}

bool Vector3D::IsZero(const Vector3D& v)
{
	return v.getComponentX() == 0.f && v.getComponentY() == 0.f && v.getComponentZ() == 0.f;
}
 

float Vector3D::getComponentZ() const
{
	return this->m_z;
}

void Vector3D::setComponentZ(const float z)
{
	this->m_z = z;
}

void Vector3D::operator+=(const Vector3D& v)
{
	this->m_x += v.getComponentX();
	this->m_y += v.getComponentY();
	this->m_z += v.getComponentZ();
}
void Vector3D::operator-=(const Vector3D& v)
{
	this->m_x -= v.getComponentX();
	this->m_y -= v.getComponentY();
	this->m_z -= v.getComponentZ();
}
void Vector3D::operator*=(const float coef)
{
	this->m_x *= coef;
	this->m_y *= coef;
	this->m_z *= coef;
}

Vector3D Vector3D::operator+(const Vector3D& v)
{
	return { this->m_x + v.getComponentX(), this->m_y + v.getComponentY(), this->m_z + v.getComponentZ() };
}
Vector3D Vector3D::operator-(const Vector3D& v)
{
	return { this->m_x - v.getComponentX(), this->m_y - v.getComponentY(), this->m_z - v.getComponentZ() };
}
Vector3D Vector3D::operator*(const float coef)
{
	return { this->m_x * coef, this->m_y * coef, this->m_z * coef };
}

std::ostream& operator<<(std::ostream& os, const Vector3D& v)
{
	os << "Vector3D(" << v.getComponentX() << ", " << v.getComponentY() << ", " << v.getComponentZ() << ")";
	return os;
}