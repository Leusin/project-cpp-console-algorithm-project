#include "AUnit.h"

#include <cmath>
#include "Engine.h"
#include "Core.h"
#include "Render/Renderer.h"
#include "Game/Debug.h"
#include "Algorithm.h"

// TEST
#include "Input.h"

AUnit::AUnit(const Vector2I& spawnPosition)
	: QEntity(spawnPosition, Color::White, "U")
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
{
	SetSortingOrder(200);
}

AUnit::~AUnit()
{
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
		FollowPath(deltaTime);
	}
}

void AUnit::Draw(Renderer& renderer)
{
	// 색상 설정
	color = (isSeleted) ? selectedColor : unitColor;

	super::Draw(renderer);

	// 디버그 정보 랜더
	if (Debug::IsDebugMode())
	{
		// 현 위치
		char debugMouse[16];
		sprintf_s(debugMouse, sizeof(debugMouse), "(%d,%d)", Position().x, Position().y);
		renderer.WriteToBuffer({ Position().x + 1, Position().y - 1 }, debugMouse, Color::LightGreen, Debug::RenderOrder() + 1);


		// 이동 경로
		if (state == AUnitState::Move)
		{
			for (int i = currentWaypointIndex; i < path.size(); ++i)
			{
				renderer.WriteToBuffer(path[i], "*", Color::LightWhite, Debug::RenderOrder() + 1);
			}

			renderer.WriteToBuffer(path.back(), "X", Color::LightRed, Debug::RenderOrder() + 1);
		}
	}
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}

void AUnit::SetMove(const Vector2I& targetPos)
{
	state = AUnitState::Move;
	path.clear();
	path = Algorithm::aStar.FindPath(GetCurrentPosition(), targetPos);

	// 새로운 경로를 받았기 때문에 인덱스 초기화
	currentWaypointIndex = 0;
}

bool AUnit::FollowPath(float deltaTime)
{
	if (state != AUnitState::Move)
	{
		return false;
	}

	// 이동 목표
	Vector2F target = path[currentWaypointIndex];

	// 이미 도착했다면
	if (path[currentWaypointIndex] == Position())
	{
		// 다음 목표로
		++currentWaypointIndex;

		// 도착지인지 확인(그리고 인덱스 밖인지 확인)
		if (currentWaypointIndex >= path.size())
		{
			currentPosition.x = (float)Position().x;
			currentPosition.y = (float)Position().y;

			state = AUnitState::Idle;
			currentWaypointIndex = 0;
			return true;
		}

		target = path[currentWaypointIndex];
	}

	// 본격적인 이동 처리

	// 이동 방향
	Vector2F toTarget = target - Position();

	// 이동 거리
	Vector2F movement = toTarget.Normalize() * stats.speed * deltaTime;

	//  TODO: 이동 가능한지(장애물이 있는지) 처리

	// 이동
	currentPosition = currentPosition + movement;

	// 그려질 위치 업데이트
	SetPosition({ (int)currentPosition.x, (int)currentPosition.y });

	// 바운드 상자 위치 업데이트
	bounds.SetPosition(GetCurrentPosition());

	return true;
}
