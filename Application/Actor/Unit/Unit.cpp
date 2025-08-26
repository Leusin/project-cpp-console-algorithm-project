#include "Unit.h"

#include <cmath>
#include "Engine.h"

Unit::Unit(const Vector2I& spawnPosition)
	: Actor("U", Color::White, spawnPosition)
	, bounds{ spawnPosition }
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
{
	SetSortingOrder(100);
}

Unit::~Unit()
{
}

void Unit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

    // 
    // TEST
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

Vector2I Unit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}
