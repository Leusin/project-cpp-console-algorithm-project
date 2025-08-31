#pragma once

#include "Actor/QEntity/QEntity.h"

#include <vector>
#include "Team.h"
#include "Utils/Timer.h"
#include "ProcessResult.h"
#include "QuadTree/Bounds.h"



/// <summary>
/// QaudTree가 관리하고 AStar를 가진 객체
/// </summary>
class AUnit : public QEntity
{
	enum class AUnitState
	{
		Idle,
		Move,
		Attack, // 추가
		Dead // 이동 불가

	};

	// Move 의 상태 (FSM 안의 FSM... )
	enum class PathStepResult 
	{
		InProgress, // 계속 이동 가능
		Success,    // 최종 목적지 도착
		Blocked     // 장애물로 이동 불가
	};

	RTTI_DECLARATIONS(AUnit, QEntity)

public: // RAII
	AUnit(const Vector2I& spawnPosition, const struct Team& team, class Map& map, class PathfindingManager& aStar, class QuadTree& qTree);
	virtual ~AUnit();

public: // EVENT

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer) override;

public: // MESSAGE

	void OnCommandToMove(const Vector2I& targetPos);
	static bool IsOverMaxCount();

public: // GET SET

	Vector2I GetCurrentPosition() const;
	Team::Type GetTeamType() const { return team.type; }
	const Team& GetTeam() const { return team; }

	void SetPath(std::vector<Vector2I> path);
	void SetIsSelected(bool val) { isSelected = val; }

	static int GetCount();
	static int GetMaxCount();

private: // METHOD

	// 상태별 틱 함수
	void TickIdle(float dt);
	void TickMove(float dt);
	void TickAttack(float dt);
	void TickDead(float dt);

	// 경로 관련
	PathStepResult AdvancePath(float dt);
	bool ShouldRetryPath(float dt);
	void RequestPath(const Vector2I& targetPos);

private: // FILD

	// 유닛 속성
	AUnitState state;
	Vector2F position; // 실수 좌표
	const float minSpeed; // 최소 속도
	const float tolerance; // 좌표 도착 판정 오차

	// 경로
	float speed;
	int currentWaypointIndex;
	int tryCount;
	const int minTry;
	Vector2I lastTarget;
	Timer blockedTimer;
	std::vector<Vector2I> path;

	// 전투
	float attackRange; // 공격 사거리: 대각선으로 공격할 수 있는 거리
	float attackCooldown; // 공격 딜레이
	float attackDamage;  // 공격력
	AUnit* targetEnemy; // 현재 공격중인 대상
	Timer attackTimer;

	// 피격
	float hp = 50.0f;             // 체력

	// 시각 효과
	bool isSelected;
	Color unitColor;
	Color selectedColor;
	Timer pathFindEffectTimer;

	// 기다림
	float maxWaitTime;
	float minWaitTime;

	// 의존성
	class Map& map;
	class QuadTree& qTree;
	const Team& team;
	class PathfindingManager& pathfindingManager;

	// 현재 존재하는 유닛 수
	static int count;
};