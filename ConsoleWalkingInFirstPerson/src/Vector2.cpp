#include "Vector2.h"

Vector2f::Vector2f()
	: X(), Y()
{}

Vector2f::Vector2f(float x, float y)
	: X(x), Y(y)
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


Vector2n::Vector2n()
	: X(), Y()
{}

Vector2n::Vector2n(int x, int y)
	: X(x), Y(y)
{}

const Vector2n Vector2n::Add(const Vector2n& lhs, const Vector2n& rhs)
{
	return { lhs.X + rhs.X, lhs.Y + rhs.Y };
}
const Vector2n Vector2n::Subtract(const Vector2n& lhs, const Vector2n& rhs)
{
	return { lhs.X - rhs.X, lhs.Y - rhs.Y };
}

const Vector2n Vector2n::operator+ (const Vector2n& other) const { return Add(*this, other); }
const Vector2n Vector2n::operator- (const Vector2n& other) const { return Subtract(*this, other); }
void Vector2n::operator+= (const Vector2n& other)
{
	auto temp = Add(*this, other);
	X = temp.X;
	Y = temp.Y;
}
void Vector2n::operator-= (const Vector2n& other)
{
	auto temp = Subtract(*this, other);
	X = temp.X;
	Y = temp.Y;
}