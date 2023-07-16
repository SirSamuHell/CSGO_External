#include "vector2d.hpp"
#include "vector3d.hpp"
#include <cmath>
 
Vector2D::Vector2D(const float x, const float y) : m_x{x}, m_y{y}
{

}

Vector2D::Vector2D(const Vector3D& v) : m_x{this->getComponentX()}, m_y{this->getComponentY()}
{

}

Vector2D Vector2D::Normalize(const Vector2D& v)
{
	const float length = v.magnitude();

	return Vector2D{v.getComponentX() / length, v.getComponentY() / length };
}

float Vector2D::Dot(const Vector2D& v1, const Vector2D& v2)
{
	return (v1.getComponentX() * v2.getComponentY()) + (v1.getComponentY() * v2.getComponentY());
}

float Vector2D::Angle(const Vector2D& v1, const Vector2D& v2)
{
	return std::acos(Vector2D::Dot(v1, v2) / (v1.magnitude() * v2.magnitude()));
}

float Vector2D::magnitude() const
{
	return std::sqrt( (this->m_x * this->m_x) + (this->m_y * this->m_y) );
}

void Vector2D::scale(const float coef)
{
	this->m_x *= coef;
	this->m_y *= coef;
}

float Vector2D::getComponentX() const
{
	return this->m_x;
}

float Vector2D::getComponentY() const
{
	return this->m_y;
}

void Vector2D::setComponentX(const float x)
{
	this->m_x = x;
}

void Vector2D::setComponentY(const float y)
{
	this->m_y = y;
}


void Vector2D::operator+=(const Vector2D& v)
{
	this->m_x += v.getComponentX();
	this->m_y += v.getComponentY();
}

void Vector2D::operator-=(const Vector2D& v)
{
	this->m_x -= v.getComponentX();
	this->m_y -= v.getComponentY();
}

void Vector2D::operator*=(const float coef)
{
	this->m_x *= coef;
	this->m_y *= coef;
}

Vector2D Vector2D::operator+(const Vector2D& v)
{
	return { this->m_x + v.getComponentX(), this->m_y + v.getComponentY()};
}

Vector2D Vector2D::operator-(const Vector2D& v)
{
	return { this->m_x - v.getComponentX(), this->m_y - v.getComponentY() };
}

Vector2D Vector2D::operator*(const float coef)
{
	return { this->m_x * coef, this->m_y * coef };
}

std::ostream& operator<<(std::ostream& os, const Vector2D& v)
{
	os << "Vector2D(" << v.getComponentX() << ", " << v.getComponentY() << ")";
	return os;
}
 