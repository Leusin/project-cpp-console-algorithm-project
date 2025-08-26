#pragma once

#include "Math/Vector2I.h"

class Bounds
{
public: // RAII

	Bounds(int x, int y, int width = 1.0f, int height = 1.0f);
	Bounds(const Vector2I& position, int width = 1.0f, int height = 1.0f);

	~Bounds() = default;

public:

	// 겹침 행성 판정 함수.
	bool Intersects(const Bounds& other) const;

public: // GET SET

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