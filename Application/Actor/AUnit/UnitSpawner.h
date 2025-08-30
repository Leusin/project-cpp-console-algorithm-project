#pragma once

#include "Team.h"
#include "Math/Vector2I.h"
#include "Utils/Timer.h"

class UnitySpawner
{
public: // RAII

	UnitySpawner(const class UnitFactory& factory, Team::Type type);
	~UnitySpawner() = default;

private: // FILD

	const class UnitFactory& factory;
	
	// 팀 타입
	Team::Type type;

	// 스포너 위치
	Vector2I spawnPosition;

	// 생성 주기
	float spawnInterval;

	Timer spawnTimer;

	// 최대 생성 수량
	int maxUnits;

	// 현재 스폰 유닛
	int spawnedCount;
};