#include "AUnit.h"

#include <cmath>
#include "Engine.h"

AUnit::AUnit(const Vector2I& spawnPosition)
	: QEntity(spawnPosition, Color::White, "U")
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
{
	SetSortingOrder(100);
}

AUnit::~AUnit()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}
