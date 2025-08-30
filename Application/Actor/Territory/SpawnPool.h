#pragma once

#include "Territory.h"

#include "Utils/Timer.h"
#include "Math/Vector2I.h"


/// <summary>
/// 일정 주기마다 유닛을 생산한다.
/// 가지고 있는 범위 보다 한칸 아래 만큼 만든다.
/// 다른 팀이 점령할 수 있다.
/// </summary>
class SpawnPool : public Territory
{
	RTTI_DECLARATIONS(SpawnPool, Territory)

public: // RAII

	SpawnPool(int id, Vector2I position, Vector2I size, class QuadTree& qTree, class Map& map, const class UnitFactory& factory);
	virtual ~SpawnPool();

public: // Event

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer)override;


private: // METHOD

	bool SpawnUnit();

private: // FILD

	// 생성 타이머
	Timer spawnTimer;

	// 생성 주기
	float spawnInterval;

	// 유닛 생성기
	const class UnitFactory& factory;

	class Map& map;

private:
	static int count;

};