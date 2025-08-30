#pragma once

#include "Math/Color.h"

struct Team
{
	// 절대 테란, 저그, 프로토스가 맞음
	enum class Type
	{
		P, 
		T, 
		Z, 
		NONE
	};

	Team(Type type, const char* img, const float& speed);

	Type type;
	const char* img;
	float speed;
	
	Color GetTeamColor() const { return GetTeamColor(type); }

	Color static GetTeamColor(Type type);
};