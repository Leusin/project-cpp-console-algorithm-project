#pragma once

#include "Actor/AUnit/Team.h"


class UnitFactory
{
public: // RAII

	UnitFactory(class AStar& aStar, class Map& map, class QuadTree& qTree);

public: // MESSAGE

	class AUnit* CreatUnit(const struct Vector2I& spawnPos, Team::Type type);

private:

	const Team none;
	const Team zerg;
	const Team terran;
	const Team protoss;

	class AStar& aStar;
	class Map& map;
	class QuadTree& qTree;

};