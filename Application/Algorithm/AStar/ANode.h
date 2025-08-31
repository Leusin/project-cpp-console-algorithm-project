#pragma once

#include "Math/Vector2I.h"

class ANode
{
public: // RAII
	ANode(int x = 0, int y = 0)
		: position{ x, y }
		, gCost{ 0 }
		, hCost{ 0 }
	{

	}

public: // OPERATOR

	Vector2I operator-(const ANode& other) const
	{
		return position - other.position;
	}

	bool operator==(const ANode& other) const
	{
		return position == other.position;
	}

public: // MESSAGE

	float FCost() const { return gCost + hCost; }

public: // DATA

	// 위치
	Vector2I position;

	// 이동 비용
	float gCost;

	// 추정 비용
	float hCost;
};