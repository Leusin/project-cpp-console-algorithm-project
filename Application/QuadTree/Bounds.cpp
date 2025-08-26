#include "Bounds.h"

Bounds::Bounds(int x, int y, int width, int height)
	: x(x), y(y), width(width), height(height)
{

}

Bounds::Bounds(const Vector2I& position, int width, int height)
	: Bounds{ position.x, position.y, width, height }
{
}

bool Bounds::Intersects(const Bounds& other) const
{
	// 다른 영역이 현재 영역의 오른쪽 바깥인지
	if (other.GetX() > MaxX())
	{
		return false;
	}

	// 다른 영역이 현재 영역의 오른쪽 바깥인지
	if (other.MaxX() < x)
	{
		return false;
	}

	// 다른 영역이 현재 영역의 아래쪽 바깥인지
	if (other.GetY() > MaxY())
	{
		return false;
	}

	// 다른 영역이 현재 영역의 위쪽 바깥인지
	if (other.MaxY() < y)
	{
		return false;
	}

	return true;
}
