#pragma once

#include "Math/Vector2I.h"

class ANode
{
public: // RAII
	ANode(int x = 0, int y = 0, ANode* parent = nullptr)
		: position{ x, y }
		, gCost{ 0 }
		, hCost{ 0 }
		, parent{ parent }
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

	// 위치
	const Vector2I position;

public: // DATA

	// 이동 비용
	float gCost;

	// 추정 비용
	float hCost;

	ANode* parent;
};