#include "Vector2.h"

Vector2f::Vector2f()
	: X(0.0f), Y(0.0f)
{}

Vector2f::Vector2f(float a, float b)
	: X(a), Y(b)
{}

const Vector2f Vector2f::Add(const Vector2f& lhs, const Vector2f& rhs)
{
	return { lhs.X + rhs.X, lhs.Y + rhs.Y };
}

const Vector2f Vector2f::Subtract(const Vector2f& lhs, const Vector2f& rhs)
{
	return { lhs.X - rhs.X, lhs.Y - rhs.Y };
}

const Vector2f Vector2f::operator+ (const Vector2f& other) const { return Add(*this, other); }
const Vector2f Vector2f::operator- (const Vector2f& other) const { return Subtract(*this, other); }
void Vector2f::operator+=(const Vector2f& other)
{
	auto temp = Add(*this, other);
	X = temp.X;
	Y = temp.Y;
}
void Vector2f::operator-=(const Vector2f& other)
{
	auto temp = Subtract(*this, other);
	X = temp.X;
	Y = temp.Y;
}
