#pragma once

#include "Math/Vector2I.h"
#include "Math/Color.h"


class Bounds
{
public: // RAII

	Bounds(int x, int y, int width = 1.0f, int height = 1.0f);
	Bounds(const Vector2I& position, int width = 1.0f, int height = 1.0f);

	~Bounds() = default;

public:

	// 겹침 행성 판정 함수.
	bool Intersects(const Bounds& other) const;

	void Draw(class Renderer& renderer, const char* img = "#", Color color = Color::Green);

public: // GET SET

	void SetPosition(int newX, int newY)
	{
		x = newX;
		y = newY;
	}

	void SetPosition(const Vector2I& newPosition)
	{
		x = newPosition.x;
		y = newPosition.y;
	}

	void SetSize(int newWidth, int newHight)
	{
		width = newWidth;
		height = newHight;
	}

	void SetSize(const Vector2I& newSize)
	{
		width = newSize.x;
		height = newSize.y;
	}

	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	int MaxX() const { return x + width; }
	int MaxY() const { return y + height; }

private:
	int x = 0;
	int y = 0;
	int width = 1;
	int height = 1;
};