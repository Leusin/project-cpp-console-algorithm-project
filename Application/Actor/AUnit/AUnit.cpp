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

void AUnit::BeginPlay()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 유닛 이동
	if (state == AUnitState::Move)
	{
		// TODO: 유닛 이동 
		bounds.SetPosition(GetCurrentPosition());
	}
}

void AUnit::Draw(Renderer& renderder)
{
	// 색상 설정
	color = (isSeleted) ? selectedColor : unitColor;

	super::Draw(renderder);

	if (Debug::IsDebugMode())
	{
		if (state == AUnitState::Move)
		{
			for (const Vector2I& position : path)
			{
				renderder.WriteToBuffer(position, "*", Color::LightGreen, Debug::RenderOrder() + 1);
			}
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
	path = aStar.FindPath(GetCurrentPosition(), targetPos);
}
