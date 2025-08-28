#pragma once

#include "Math/Color.h"

struct UnitStats
{
	const char* icon;
	Color color = Color::Blue;
	float speed = 1.3f;

	UnitStats(const char* icon = "U", Color color = Color::White, float speed = 10)
		: icon(icon), color(color), speed(speed)
	{

	}
};