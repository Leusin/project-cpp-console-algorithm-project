#include "AUnit.h"

#include <cmath>
#include "Core.h"
#include "Input.h"
#include "Engine.h"
#include "AStar/AStar.h"
#include "Level/Map/Map.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "Utils/DebugManage.h"


AUnit::AUnit(const Vector2I& spawnPosition, const Team& team, Map& map, AStar& aStar, QuadTree& qTree)
	: QEntity(spawnPosition, team.color, team.img)
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
	, unitColor{ team.color }
	, currentWaypointIndex{ 0 }
	, tryCount{ 0 }
	, minTry{ 3 }
	, map{ map }
	, aStar{ aStar }
	, qTree{ qTree }
	, team{ team }
{
	SetSortingOrder(200);

	// 현위치 표시
	map.SetOccupiedMap(spawnPosition, true);

	// 길가다 막히면 잠시 기달림
	blockedTimer.SetTargetTime(0.1f);
}

void AUnit::BeginPlay()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 유닛 이동
	if (state == AUnitState::Move)
	{
		// 경로를 따라 이동 시도
		ProcessResult result = FollowPath(deltaTime);
		if (result == ProcessResult::Success)
		{
			state = AUnitState::Idle;
		}
		else if (result == ProcessResult::Failed)
		{
			if (tryCount > 0)
			{
				blockedTimer.Tick(deltaTime);

				if (blockedTimer.IsTimeout())
				{
					SetMove(lastTarget, aStar, map);

					--tryCount;

					blockedTimer.Reset();
				}
			}
			else
			{
				state = AUnitState::Idle;
			}
		}
	}
	else if (state == AUnitState::Idle)
	{
		map.SetOccupiedMap(Position(), true);
	}
}

void AUnit::Draw(Renderer& renderer)
{
	// 색상 설정
	color = (isSeleted) ? selectedColor : unitColor;

	super::Draw(renderer);

	// 이동 경로
	if (state == AUnitState::Move && !path.empty())
	{
		for (int i = currentWaypointIndex; i < path.size(); ++i)
		{
			renderer.WriteToBuffer(path[i], "*", Color::White, DebugManage::RenderOrder() + 1);
		}

		renderer.WriteToBuffer(path.back(), "X", unitColor, DebugManage::RenderOrder() + 2);
	}

	// 디버그 정보 랜더
	if (DebugManage::IsDebugMode())
	{
		// 현 위치
		char debugMouse[16];
		sprintf_s(debugMouse, sizeof(debugMouse), "(%d,%d)", Position().x, Position().y);
		renderer.WriteToBuffer({ Position().x, Position().y + 1 }, debugMouse, Color::LightGreen, DebugManage::RenderOrder() + 3);
	}
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}

void AUnit::SetMove(const Vector2I& targetPos, AStar& aStar, const Map& map)
{
	state = AUnitState::Move;

	lastTarget = targetPos;   // 기억

	path.clear();
	path = aStar.FindPath(GetCurrentPosition(), targetPos, map);

	// 길찾기 실패 시 시도 횟수
	tryCount = (int)path.size() * minTry;

	// 새로운 경로를 받았기 때문에 인덱스 초기화
	currentWaypointIndex = 0;
}

ProcessResult AUnit::FollowPath(float deltaTime)
{
	if (state != AUnitState::Move || path.empty())
	{
		return ProcessResult::Failed;
	}

	Vector2F fTarget = path[currentWaypointIndex];
	Vector2F toTarget = fTarget - currentPosition;
	float dist = toTarget.Magnitude();

	// 목표 도착 체크 (정수 == 비교 대신 거리 기반)
	if (dist < 1e-6f)
	{
		++currentWaypointIndex;
		if (currentWaypointIndex >= path.size())
		{
			return ProcessResult::Success; // 최종 목적지 도착
		}
		fTarget = path[currentWaypointIndex];
		toTarget = fTarget - currentPosition;
		dist = toTarget.Magnitude();
	}

	// 이미 도착
	if (dist < 1e-6f) // 0.000001
	{
		return ProcessResult::Success;
	}

	// 이동 속도 계산
	Vector2I iPosition = Position();
	float terrainWeight = map.GetWeightMap(iPosition);
	if (terrainWeight <= 0.0f)
	{
		return ProcessResult::Failed; // 이동 불가
	}

	float finalSpeed = terrainWeight * team.speed;
	if (finalSpeed < 0.01f) finalSpeed = 0.01f; // 최소 속도 보정

	// 이동량 계산 (목표를 지나치지 않도록 보정)
	Vector2F movement = (toTarget / dist)/* = toTarget.Normaize*/ * finalSpeed * deltaTime;
	if (movement.Magnitude() > dist)
	{
		movement = toTarget; // 목표까지만 이동
	}

	// 최종 좌표 업데이트
	Vector2F fNextPos = currentPosition + movement;
	Vector2I iNextPos((int)round(fNextPos.x), (int)round(fNextPos.y));
	fNextPos.RoundToVector2I(iNextPos);

	if (iPosition != iNextPos && !map.CanMove(iNextPos))
	{
		return ProcessResult::Failed;
	}

	currentPosition = fNextPos;
	SetPosition(iNextPos);
	map.SetOccupiedMap(iPosition, false);
	map.SetOccupiedMap(iNextPos, true);
	bounds.SetPosition(GetCurrentPosition());

	return ProcessResult::InProgress;
}