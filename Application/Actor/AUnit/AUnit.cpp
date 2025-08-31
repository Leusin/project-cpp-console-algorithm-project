#include "AUnit.h"

#include <cmath>
#include "Core.h"
#include "Input.h"
#include "Engine.h"
#include "Utils/Utils.h"
#include "Level/Map/Map.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "Utils/DebugManage.h"
#include "AStar/PathfindingManager.h"

int AUnit::count = 0;

bool AUnit::IsOverMaxCount()
{
	return GetMaxCount() <= count;
}

int AUnit::GetCount()
{
	return count;
}

int AUnit::GetMaxCount()
{
	return (int)(Engine::Width() * Engine::Height() / 100);
}

AUnit::AUnit(const Vector2I& spawnPosition, const Team& team, Map& map, PathfindingManager& aStar, QuadTree& qTree)
	: QEntity(spawnPosition, team.GetTeamColor(), team.img)
	// 유닛 속성
	, state{ State::Idle }
	, position{ (float)spawnPosition.x, (float)spawnPosition.y }
	, minSpeed{ 0.01f }
	, tolerance{ 1e-6f /*= 0.000001*/ }
	// 경로
	, speed{ 10.0f }
	, tryCount{ 0 }
	, currentWaypointIndex{ 0 }
	, minTry{ 3 }
	, lastTarget{}
	// 전투
	, attackRange{ 1.5f }
	, searchRange{ 7 }
	, attackCooldown{ 0.5f }
	, attackDamage{ 10.0f }
	, targetEnemy{ nullptr }
	// 피격
	, hp{ 50.0f }
	, isDamaged{ false }
	, damagedCooldown{ 0.55f }
	// 시각 효과
	, isSelected{ false }
	, unitColor{ team.GetTeamColor() }
	, damagedColor{ Color::Red }
	, onAttackColor{ (Color)((int)team.GetTeamColor() | (int)Color::Intensity) }
	, selectedColor{ Color::LightGreen }
	// 경로 재검색 기다림
	, minWaitTime{ 0.001f }
	, maxWaitTime{ 0.125f }
	// 의존성
	, map{ map }
	, team{ team }
	, qTree{ qTree }
	, pathfindingManager{ aStar }
{
	++count;

	// 랜더 순서
	SetSortingOrder(200);

	// 현위치 표시
	map.SetOccupiedMap(spawnPosition, true);

	// 길가다 막히면 잠시 기달림
	blockedTimer.SetTargetTime(Utils::RandomFloat(minWaitTime, maxWaitTime));

	// 종족치(?) 반영
	float teamBonous = team.bonus;
	hp += hp * (1.0f - teamBonous);
	attackCooldown += attackCooldown * (1.0f - teamBonous);
	attackDamage += attackDamage *(1.0f - teamBonous);
	damagedCooldown *= teamBonous;

	// 공격 타이머 설정
	attackTimer.SetTargetTime(attackCooldown);
	// 피격 타이머 설정
	damagedTimer.SetTargetTime(damagedCooldown);
}

AUnit::~AUnit()
{
	pathfindingManager.CancelRequest(this);

	--count;
}

void AUnit::BeginPlay()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	switch (state)
	{
	case State::Idle:   TickIdle(deltaTime);   break;
	case State::Move:   TickMove(deltaTime);   break;
	case State::Attack: TickAttack(deltaTime); break;
	case State::Dead:   TickDead(deltaTime);   break;
	}

	if (isDamaged)
	{
		damagedTimer.Tick(deltaTime);
		if (damagedTimer.IsTimeout())
		{
			isDamaged = false;
		}
	}
}

void AUnit::Draw(Renderer& renderer)
{
	// 색상 설정
	color = (isSelected) ? selectedColor : unitColor;
	color = (state == State::Attack) ? onAttackColor : color;
	color = (isDamaged) ? damagedColor : color;

	super::Draw(renderer);

	// 이동 경로
	if (state == State::Move && !path.empty())
	{
		int pathDrawIndex = currentWaypointIndex + (int)(pathFindEffectTimer.GetElapsedTime() * speed * team.bonus * 2.f);
		for (int i = pathDrawIndex; i < path.size(); ++i)
		{
			renderer.WriteToBuffer(path[i], "#", Color::White, DebugManage::RenderOrder() + 1);
		}

		renderer.WriteToBuffer(path.back(), "@", unitColor, DebugManage::RenderOrder() + 2);
	}

#ifdef _DEBUG

	// 디버그 정보 랜더
	DebugManage::Mode mode = DebugManage::GetMode();
	if (mode == DebugManage::Mode::Position || mode == DebugManage::Mode::ALL)
	{
		// 현 위치
		char debugMouse[16];
		sprintf_s(debugMouse, sizeof(debugMouse), "(%d,%d)", Position().x, Position().y);
		renderer.WriteToBuffer({ Position().x, Position().y + 1 }, debugMouse, Color::LightGreen, DebugManage::RenderOrder() + 3);
	}
	else if (mode == DebugManage::Mode::Path || mode == DebugManage::Mode::ALL)
	{
		// 이동 경로
		if (state == State::Move && !path.empty())
		{
			for (int i = currentWaypointIndex; i < path.size(); ++i)
			{
				renderer.WriteToBuffer(path[i], "#", Color::White, DebugManage::RenderOrder() + 1);
			}

			renderer.WriteToBuffer(path.back(), "@", unitColor, DebugManage::RenderOrder() + 2);
		}
	}

#endif
}

void AUnit::OnDestroy()
{
	// 죽으면 맵 점유 해제
	map.SetOccupiedMap(GetCurrentPosition(), false);
}

void AUnit::OnCommandToMove(const Vector2I& targetPos)
{
	lastTarget = targetPos;
	tryCount = minTry;
	RequestPath(targetPos);
}

void AUnit::TakeDamage(float dmg)
{
	if (isDamaged)
	{
		return;
	}

	isDamaged = true;
	damagedTimer.Reset();

	hp -= dmg;
	if (hp <= 0.0f)
	{
		hp = 0.0f;
		state = State::Dead;
	}
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(position.x), (int)round(position.y));
}

// 경로 할당 시 처리하는 것 여러가지
void AUnit::SetPath(std::vector<Vector2I> path)
{
	this->path = std::move(path);
	state = State::Move;
	currentWaypointIndex = 0;
	pathFindEffectTimer.Reset();
}

void AUnit::TickIdle(float dt)
{
	map.SetOccupiedMap(GetCurrentPosition(), true);

	// 근처 적 찾기
	Vector2I point = { Position() - Vector2I{searchRange / 2, searchRange / 2} };
	if (std::vector<QEntity*> nearby; qTree.Query(Bounds(point, searchRange, searchRange), nearby))
	{
		AUnit* closest = nullptr;
		float closestDist = (float)(Engine::Width() * Engine::Height());

		for (QEntity* entity : nearby)
		{
			if (!entity->As<AUnit>())
			{
				continue;
			}

			AUnit* unit = (AUnit*)entity;

			// 같은 팀인지 검사
			if (unit->GetTeamType() == team.type)
			{
				continue;
			}

			float dist = Vector2F(unit->Position() - Position()).Magnitude();
			if (dist < closestDist && dist <= searchRange)
			{
				closest = unit;
				closestDist = dist;
			}
		}

		if (closest != nullptr)
		{
			targetEnemy = closest;
			state = State::Attack;
		}
	}

}

// 이동및 경로 재탐색
void AUnit::TickMove(float dt)
{
	switch (AdvancePath(dt))
	{
	case PathStepResult::InProgress:
	{
		pathFindEffectTimer.Tick(dt);
	}
	break;
	case PathStepResult::Success:
	{
		state = State::Idle;
	}
	break;

	case PathStepResult::Blocked:
	{
		// 시도 횟수 종료
		if (tryCount <= 0)
		{
			state = State::Idle;
		}

		if (ShouldRetryPath(dt))
		{
			RequestPath(lastTarget);
		}
	}
	break;
	}
}

void AUnit::TickAttack(float dt)
{
	if (targetEnemy == nullptr || targetEnemy->IsDead())
	{
		state = State::Idle;
		return;
	}

	// 사거리 체크
	Vector2F toTarget = targetEnemy->Position() - Position();
	float dist = toTarget.Magnitude();
	if (dist > attackRange)
	{
		// 다시 쫓아감
		RequestPath(targetEnemy->GetCurrentPosition());
		state = State::Move;
		return;
	}

	// 공격 타이머
	attackTimer.Tick(dt);
	if (attackTimer.IsTimeout())
	{
		targetEnemy->TakeDamage(attackDamage);

		// 쿨타임 초기화
		attackTimer.Reset();
	}
}

void AUnit::TickDead(float dt)
{
	// 길찾기 요청 해제
	pathfindingManager.CancelRequest(this);

	// 사망 처리
	Destroy();
}

AUnit::PathStepResult AUnit::AdvancePath(float dt)
{
	if (path.empty() || currentWaypointIndex >= (int)path.size())
	{
		return PathStepResult::Blocked;
	}

	Vector2F fTarget = path[currentWaypointIndex];
	Vector2F toTarget = fTarget - position;
	float dist = toTarget.Magnitude();

	// 목표 도착 체크
	if (dist < tolerance)
	{
		++currentWaypointIndex;

		// 최종 목적지 도착
		if (currentWaypointIndex >= path.size())
		{
			return PathStepResult::Success;
		}
		fTarget = path[currentWaypointIndex];
		toTarget = fTarget - position;
		dist = toTarget.Magnitude();
	}

	// 이미 도착
	if (dist < tolerance)
	{
		return PathStepResult::Success;
	}

	Vector2I iPosition = Position(); // 정수형 위치 가져오기
	float terrainWeight = map.GetWeightMap(iPosition);

	// 이동할 수 없는 지형
	if (terrainWeight <= 0.0f)
	{
		return PathStepResult::Blocked;
	}

	// 최종 이속
	float finalSpeed = terrainWeight * speed * team.bonus;

	// 최소 속도 보정
	if (finalSpeed < minSpeed)
	{
		finalSpeed = minSpeed;
	}

	// 이동량 계산 (목표를 지나치지 않도록 보정)
	Vector2F movement = (toTarget / dist)/* = toTarget.Normaize*/ * finalSpeed * dt;
	if (movement.Magnitude() > dist)
	{
		movement = toTarget; // 목표까지만 이동
	}

	// 최종 좌표 업데이트
	Vector2F fNextPos = position + movement;
	Vector2I iNextPos((int)round(fNextPos.x), (int)round(fNextPos.y));
	fNextPos.RoundToVector2I(iNextPos);

	// 다음 위치는 이동 불가한 경우
	if (iPosition != iNextPos && !map.CanMove(iNextPos))
	{
		return PathStepResult::Blocked;
	}

	position = fNextPos;
	SetPosition(iNextPos);
	map.SetOccupiedMap(iPosition, false);
	map.SetOccupiedMap(iNextPos, true);
	bounds.SetPosition(GetCurrentPosition());

	// 이동 중
	return PathStepResult::InProgress;
}

// 이동 실패시 호출
// 이동 횟수가 남았는지, 대기 시간이 남았는지
bool AUnit::ShouldRetryPath(float dt)
{
	blockedTimer.Tick(dt);

	if (blockedTimer.IsTimeout())
	{
		// 시도 횟수 감소
		--tryCount;

		// 기다리는 시간 재설정
		blockedTimer.SetTargetTime(Utils::RandomFloat(maxWaitTime, minWaitTime));
		blockedTimer.Reset();

		return true;
	}

	return false;
}

// 길찾기 요청
void AUnit::RequestPath(const Vector2I& targetPos)
{
	path.clear();
	pathfindingManager.AddRequest(this, GetCurrentPosition(), targetPos, map);
}