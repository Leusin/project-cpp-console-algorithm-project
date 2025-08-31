#pragma once

#include "Actor/AUnit/Team.h"


class UnitFactory
{
public: // RAII

	UnitFactory(class PathfindingManager& aStar, class Map& map, class QuadTree& qTree);

public: // MESSAGE

	class AUnit* CreatUnit(const struct Vector2I& spawnPos, Team::Type type) const;

public:

	const Team none;
	const Team zerg;
	const Team terran;
	const Team protoss;

private:

	class PathfindingManager& aStar;
	class Map& map;
	class QuadTree& qTree;

};