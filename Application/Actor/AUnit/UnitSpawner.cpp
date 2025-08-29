#include "UnitSpawner.h"

#include "UnitFactory.h"

UnitySpawner::UnitySpawner(const UnitFactory& factory, Team::Type type = Team::Type::NONE)
	: factory{ factory }
	, type{ type }
{
}
