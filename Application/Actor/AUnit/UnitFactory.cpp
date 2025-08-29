#include "UnitFactory.h"

#include "Math/Vector2I.h"
#include "Math/Color.h"
#include "Actor/AUnit/AUnit.h"
#include "AStar/AStar.h"
#include "Level/Map/Map.h"
#include "QuadTree/QuadTree.h"

UnitFactory::UnitFactory(class AStar& aStar, class Map& map, class QuadTree& qTree)
	: none{}
	, zerg{ Team::Type::Z, Color::Magenta , "Z", 10.f }
	, terran{ Team::Type::Z, Color::Cyan, "T", 8.f }
	, protoss{ Team::Type::Z, Color::Yellow, "P", 7.f }
	, aStar{ aStar }
	, map{ map }
	, qTree{ qTree }
{
}

AUnit* UnitFactory::CreatUnit(const Vector2I& spawnPos, Team::Type type)
{
	switch (type)
	{
	case Team::Type::T:
		return new AUnit(spawnPos, terran, map, aStar, qTree);
		break;
	case Team::Type::Z:
		return new AUnit(spawnPos, zerg, map, aStar, qTree);
		break;
	case Team::Type::P:
		return new AUnit(spawnPos, protoss, map, aStar, qTree);
		break;
	default:
		return new AUnit(spawnPos, none, map, aStar, qTree);
		break;
	}

	return nullptr;
}
