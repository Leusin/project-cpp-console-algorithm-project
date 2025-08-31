#pragma once

#include <vector>
#include "Core.h"
#include "RTTI.h"

class Actor;

/// <summary>
/// - 액터들을 관리한다.
/// </summary>
class Engine_API Level : public RTTI
{
	RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();

	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void SlowTick(float deltaTime);
	virtual void Draw(class Renderer& renderder);

	void AddActor(Actor* newActor); // 레벨에 액터 추가
	void DestroyActor(Actor* newActor); // 액터 삭제
	void ProcessAddAndDestroyActors(); // 추가 또는 삭제 요청된 액터 처리

	const std::vector<Actor*>& GetActors() const;

protected:
	virtual void OnActorDestroyed(Actor* actor) {}

protected:
	std::vector<Actor*> actors; // 레벨에 배치된 모든 액터 관리

private:
	std::vector<Actor*> addRequestedActors; // 추가 요청된 액터를 관리하는 배열
	std::vector<Actor*> destroyRequstedActors; // 삭제 요청된 배치된 모든 액터 관리
};
