#include "AUnit.h"

#include <cmath>
#include "Engine.h"
#include "AStar/AStar.h"
#include "Core.h"
#include "Render/Renderer.h"

// TEST
#include "Input.h"

AUnit::AUnit(const Vector2I& spawnPosition, class AStar& aStar)
	: QEntity(spawnPosition, Color::White, "U")
	, currentPosition{ (float)spawnPosition.x, (float)spawnPosition.y }
	, aStar{ aStar }
{
	SetSortingOrder(100);
}

AUnit::~AUnit()
{
}

void AUnit::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	path.clear();
	path = aStar.FindPath(GetCurrentPosition(), {Input::Get().GetMouseX(), Input::Get().GetMouseY() });
	//path = aStar.FindPath(GetCurrentPosition(), { 33, 33 });
}

void AUnit::Draw(Renderer& renderder)
{
	super::Draw(renderder);

	for (const Vector2I& position : path)
	{
		renderder.WriteToBuffer(position, "*", Color::Green, 80);
	}
}

Vector2I AUnit::GetCurrentPosition() const
{
	return Vector2I((int)round(currentPosition.x), (int)round(currentPosition.y));
}
