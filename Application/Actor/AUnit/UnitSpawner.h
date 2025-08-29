#pragma once

#include "Team.h"

class UnitySpawner
{
public: // RAII

	UnitySpawner(const class UnitFactory& factory, Team::Type type);
	~UnitySpawner() = default;

private: // FILD

	const class UnitFactory& factory;
	
	Team::Type type;
};