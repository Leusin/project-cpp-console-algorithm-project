#include "UnitFactory.h"

#include "Math/Vector2I.h"
#include "Math/Color.h"
#include "Actor/AUnit/AUnit.h"
#include "AStar/AStar.h"
#include "Level/Map/Map.h"
#include "QuadTree/QuadTree.h"

UnitFactory::UnitFactory(class AStar& aStar, class Map& map, class QuadTree& qTree)
	: none{ Team::Type::NONE , "U" ,  13.0f }
	, zerg{ Team::Type::Z, "Z", 13.5f }
	, terran{ Team::Type::T, "T", 13.0f }
	, protoss{ Team::Type::P, "P", 12.8f }
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
		return nullptr;
		break;
	}

	return nullptr;
}
