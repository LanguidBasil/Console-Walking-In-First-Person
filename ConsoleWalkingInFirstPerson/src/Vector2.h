#pragma once

struct Vector2f
{
	float X;
	float Y;

	Vector2f();
	Vector2f(float a, float b);

	static const Vector2f Add(const Vector2f& lhs, const Vector2f& rhs);
	static const Vector2f Subtract(const Vector2f& lhs, const Vector2f& rhs);

	const Vector2f operator+ (const Vector2f& other) const;
	const Vector2f operator- (const Vector2f& other) const;
	void operator+= (const Vector2f& other);
	void operator-= (const Vector2f& other);
};

struct Vector2n
{
	int X;
	int Y;
};