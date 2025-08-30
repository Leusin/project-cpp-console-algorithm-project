#include "Team.h"

Team::Team(Type type = Type::NONE, const char* img = "U", const float& speed = 13.0f)
	: type{ type }
	, img{ img }
	, speed{ speed }
{
}

Color Team::GetTeamColor(Type type)
{
	switch (type)
	{
	case Team::Type::T:
		return Color::Cyan;
	case Team::Type::Z:
		return Color::Magenta;
	case Team::Type::P:
		return Color::Yellow;
	default:
		return Color::White;
		break;
	}
}
