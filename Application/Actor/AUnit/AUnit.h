#pragma once

#include "Actor/QEntity/QEntity.h"

#include <vector>
#include "QuadTree/Bounds.h"
#include "UnitStats.h"

enum class AUnitState
{
	Idle,
	Move
};

/// <summary>
/// QaudTree가 관리하고 AStar를 가진 객체
/// </summary>
class AUnit : public QEntity
{
	RTTI_DECLARATIONS(AUnit, QEntity)

public: // RAII
	AUnit(const Vector2I& spawnPosition);
	virtual ~AUnit();

public: // EVENT
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer) override;

public: // MESSAGE

public: // GET SET

	Vector2I GetCurrentPosition() const;
	void SetIsSelected(bool val) { isSeleted = val; }

	void SetMove(const Vector2I& targetPos, class AStar& aStar);

private: // METHOD

	bool FollowPath(float deltaTime);

private: // FILD

	// 유닛의 현재 상태
	AUnitState state = AUnitState::Idle;

	UnitStats stats;
	
	// 현재 위치(실수 좌표) - 내보낼땐 정수 좌표로 변환
	Vector2F currentPosition;

	// 길찾기 경로
	std::vector<Vector2I> path;
	int currentWaypointIndex = 0;

	// 선택 되었는지
	bool isSeleted = false;

	// 선택되었을 때 색
	Color selectedColor = Color::LightGreen;

	// 원래 색
	Color unitColor = Color::White;
};