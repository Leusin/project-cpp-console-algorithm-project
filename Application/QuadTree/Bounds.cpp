#include "Bounds.h"
#include "Render/Renderer.h"

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

void Bounds::Draw(Renderer& renderer)
{
	Color color = Color::LightCyan;

	int renderOrder = 300;

	// 현재 노드 경계 그리기
	for (int ix = 1; ix < width; ++ix)
	{
		// Top
		renderer.WriteToBuffer({ x + ix, y }, "-", color, renderOrder);
		// Bottom
		renderer.WriteToBuffer({ x + ix, y + height }, "-", color, renderOrder);
	}

	for (int iy = 1; iy < height; ++iy)
	{
		// Left
		renderer.WriteToBuffer({ x, y + iy }, "|", color, renderOrder);
		// Right
		renderer.WriteToBuffer({ x + width, y + iy }, "|", color, renderOrder);
	}

	renderer.WriteToBuffer({ x, y }, "+", color, renderOrder); // Top-left
	renderer.WriteToBuffer({ x + width, y }, "+", color, renderOrder); // Top-right
	renderer.WriteToBuffer({ x, y + height }, "+", color, renderOrder); // Bottom-left
	renderer.WriteToBuffer({ x + width, y + height }, "+", color, renderOrder); // Bottom-right
}