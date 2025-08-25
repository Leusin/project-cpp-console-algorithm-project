#include "Vector2I.h"

#include "Vector2F.h"

Vector2I Vector2I::Zero = Vector2I(0, 0);
Vector2I Vector2I::One = Vector2I(1, 1);
Vector2I Vector2I::Right = Vector2I(1, 0);

Vector2I::Vector2I(int x, int y)
	: x(x)
	, y(y)
{
}

Vector2I Vector2I::operator+(const Vector2I& other) const
{
	return { x + other.x, y + other.y };
}

Vector2I Vector2I::operator-(const Vector2I& other) const
{
	return { x - other.x, y - other.y };
}

bool Vector2I::operator==(const Vector2I& other) const
{
	return x == other.x && y == other.y ;
}

bool Vector2I::operator!=(const Vector2I& other) const
{
	return x != other.x || y != other.y;
}

Vector2I::operator COORD()
{
	COORD coord;
	coord.X = (short)x;
	coord.Y = (short)y;

	return coord;
}

Vector2I::operator COORD() const
{
	COORD coord;
	coord.X = (short)x;
	coord.Y = (short)y;

	return coord;
}

Vector2I::operator Vector2F()
{
	return { (float)x, (float)y };
}

Vector2I::operator Vector2F() const
{
	return { (float)x, (float)y };
}
