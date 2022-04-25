#pragma once

struct Vector2n;

struct Vector2f
{
	float X;
	float Y;

	Vector2f();
	Vector2f(float x, float y);
	Vector2f(const Vector2n& other);

	static const Vector2f	Add(const Vector2f& lhs, const Vector2f& rhs);
	static const Vector2f	Subtract(const Vector2f& lhs, const Vector2f& rhs);
	static const bool		Equals(const Vector2f& lhs, const Vector2f& rhs);

	const Vector2f	operator+  (const Vector2f& other) const;
	const Vector2f	operator-  (const Vector2f& other) const;
	void			operator+= (const Vector2f& other);
	void			operator-= (const Vector2f& other);
	const bool		operator== (const Vector2f& other) const;
};

struct Vector2n
{
	int X;
	int Y;

	Vector2n();
	Vector2n(int x, int y);

	static const Vector2n	Abs(const Vector2n& vector);

	static const Vector2n	Add(const Vector2n& lhs, const Vector2n& rhs);
	static const Vector2n	Subtract(const Vector2n& lhs, const Vector2n& rhs);
	static const bool		Equals(const Vector2n& lhs, const Vector2n& rhs);

	const Vector2n	operator+  (const Vector2n& other) const;
	const Vector2n	operator-  (const Vector2n& other) const;
	void			operator+= (const Vector2n& other);
	void			operator-= (const Vector2n& other);
	const bool		operator== (const Vector2n& other) const;
};