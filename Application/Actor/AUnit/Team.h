#pragma once

#include "Math/Color.h"

struct Team
{
	// 절대 테란, 저그, 프로토스가 맞음
	enum class Type
	{
		T, Z, P, NONE
	};

	Type type = Type::NONE;
	Color color = Color::White;
	const char* img = "U";
	float speed = 13.0f;
};