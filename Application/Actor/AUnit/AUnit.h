#pragma once

#include "Actor/QEntity/QEntity.h"

#include <vector>
#include "QuadTree/Bounds.h"
#include "UnitStats.h"

/// <summary>
/// QaudTree가 관리하고 AStar를 가진 객체
/// </summary>
class AUnit : public QEntity
{
	RTTI_DECLARATIONS(AUnit, QEntity)

public: // RAII
	AUnit(const Vector2I& spawnPosition, class AStar& aStar);
	virtual ~AUnit();

public:
	//virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderder) override;

public: // GET SET

	Vector2I GetCurrentPosition() const;

private: // METHOD


private: // FILD

	UnitStats stats;
	
	// 현재 위치(실수 좌표) - 내보낼땐 정수 좌표로 변환
	Vector2F currentPosition;

	// 
	class AStar& aStar;

	// 
	std::vector<Vector2I> path;
};