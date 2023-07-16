#ifndef VECTOR2D_HPP_INCLUDED 
#define VECTOR2D_HPP_INCLUDED

#include <ostream>

class Vector3D;

class Vector2D
{
public:
	Vector2D() = default;
	Vector2D(const Vector2D&) = default;
	Vector2D(const float, const float);
	Vector2D(const Vector3D&);
	~Vector2D() = default;

public:
	static Vector2D Normalize(const Vector2D& v);
	static float Dot(const Vector2D&, const Vector2D&);
	static float Angle(const Vector2D&, const Vector2D&);

public:
	float magnitude() const;
 	void scale(const float coef);

 
	float getComponentX() const;
	float getComponentY() const;
	void setComponentX(const float);
	void setComponentY(const float);

public:
 	void operator+=(const Vector2D&);
	void operator-=(const Vector2D&);
	void operator*=(const float);
	Vector2D operator+(const Vector2D&);
	Vector2D operator-(const Vector2D&);
	Vector2D operator*(const float);

	friend std::ostream& operator<<(std::ostream&, const Vector2D&);
 	
protected:
	float m_x{0.0f};
	float m_y{0.0f};
};

#endif 