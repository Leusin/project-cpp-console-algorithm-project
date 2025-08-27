#include "AUnit.h"

#include <cmath>
#include "Engine.h"
#include "AStar/AStar.h"
#include "Core.h"
#include "Render/Renderer.h"
#include "Debug/Debug.h"

// TEST
#include "Input.h"

AUnit::AUnit(const Vector2I& spawnPosition, class AStar& aStar)
	: QEntity(spawnPosition, Color::White, "U")
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
	, aStar{ aStar }
{
	SetSortingOrder(200);
}

AUnit::~AUnit()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	//
	// 길찾기 TEST
	//

	path.clear();
	path = aStar.FindPath(GetCurrentPosition(), { Input::Get().GetMouseX(), Input::Get().GetMouseY() });

	// 
	// 쿼드 트리 TEST
	// 

	static float directionX = 1.0f; // 1: 오른쪽, -1: 왼쪽
	// 이동할 거리 계산 (속도 * 시간)
	float distance = stats.speed * deltaTime;

	// 현재 위치를 업데이트
	currentPosition.x += distance * directionX;

	// 화면 경계 체크 및 방향 전환
	if (currentPosition.x > Engine::Width())
	{
		// 화면 오른쪽 끝에 도달하면 방향을 왼쪽으로 바꿈
		currentPosition.x = Engine::Width();
		directionX = -1.0f;
	}
	else if (currentPosition.x < 0)
	{
		// 화면 왼쪽 끝에 도달하면 방향을 오른쪽으로 바꿈
		currentPosition.x = 0;
		directionX = 1.0f;
	}

	// 그려질 위치 업데이트
	SetPosition(GetCurrentPosition());
	// 경계 상자 업데이트
	bounds.SetPosition(GetCurrentPosition());
}

void AUnit::Draw(Renderer& renderder)
{
	super::Draw(renderder);

	if (Debug::IsDebugMode())
	{
		for (const Vector2I& position : path)
		{
			renderder.WriteToBuffer(position, "*", Color::Green, Debug::RenderOrder() + 1);
		}
	}
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}
