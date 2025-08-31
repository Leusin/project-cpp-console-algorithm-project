#pragma once

#include "Actor/QEntity/QEntity.h"

#include <vector>
#include "Team.h"
#include "Utils/Timer.h"
#include "ProcessResult.h"
#include "QuadTree/Bounds.h"

enum class AUnitState
{
	Idle,
	Move,
	Attack // 추가
};

/// <summary>
/// QaudTree가 관리하고 AStar를 가진 객체
/// </summary>
class AUnit : public QEntity
{
	enum class PathResult
	{
		InProgress, // 계속 진행 중
		Success,    // 최종 목적지 도착
		Blocked      // 이동 불가
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

public: // GET SET

	static int GetCount();

	static int GetMaxCount();

	static bool IsOverMaxCount();

	Vector2I GetCurrentPosition() const;
	void SetIsSelected(bool val) { isSeleted = val; }

	void OnCommandToMove(const Vector2I& targetPos);

	Team::Type GetTeamType() const { return team.type; }

	void SetPath(std::vector<Vector2I> path);

private: // METHOD

	ProcessResult FollowPath(float deltaTime);

	void SetNewPath(const Vector2I& targetPos);

private: // FILD

	// 유닛의 현재 상태
	AUnitState state = AUnitState::Idle;

	// 현재 위치(실수 좌표) - 내보낼땐 정수 좌표로 변환
	Vector2F currentPosition;

	// 최소한의 속도
	const float minSpeed;

	// 길찾기 경로
	std::vector<Vector2I> path;
	int currentWaypointIndex; // 현재 탐색중인 인덱스
	Vector2I lastTarget; // 최근 목적지
	int tryCount; // 현재 길찾기 시도 횟수
	const int minTry; // 최소 시도 횟수
	Timer blockedTimer;
	float maxWiatTime;
	float minWiatTime;
	const float tolerance; // 최소 오차, 기다리기 최소 
	Timer effectTimer;


	// 선택 되었는지
	bool isSeleted = false;

	// 선택되었을 때 색
	Color selectedColor = Color::LightGreen;

	// 원래 색
	Color unitColor = Color::White;

	// 공격 관련
	float attackRange;      // 사거리
	float attackCooldown;   // 공격 간격
	Timer attackTimer;             // 공격 타이머
	AUnit* targetEnemy;  // 현재 공격중인 적

	// 의존성
	class Map& map;
	class QuadTree& qTree;
	const Team& team;
	class PathfindingManager& pathfindingManager;

	// 현재 존재하는 유닛 수
	static int count;
};