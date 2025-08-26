#include "Unit.h"

Unit::Unit(const Vector2I& spawnPosition)
	: Actor("U", Color::White, spawnPosition)
	, bounds{ spawnPosition }
{
	SetSortingOrder(100);
}

Unit::~Unit()
{
}
