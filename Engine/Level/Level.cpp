#include "Level.h"

#include <iostream>
#include "Utils/Utils.h"
#include "Actor/Actor.h"
#include "Render/Renderer.h"

Level::Level()
{
}

Level::~Level()
{
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}
}

void Level::BeginPlay()
{
	for (Actor* const actor : actors)
	{
		if (!actor->IsActive() || actor->IsExpired()) // 비활성이거나 폐기 요청중이라면 건너뛰기 
		{
			continue;
		}

		if (actor->HasBegonPlay()) // 이미 호출된 객체 건너뛰기
		{
			continue;
		}

		actor->BeginPlay();
	}
}

void Level::Tick(float deltaTime)
{
	for (Actor* const actor : actors)
	{
		actor->Tick(deltaTime);
	}
}

void Level::SlowTick(float deltaTime)
{

}

void Level::Draw(Renderer& renderder)
{
	for (Actor* const actor : actors)
	{
		actor->Draw(renderder); // Draw Call
	}
}

void Level::AddActor(Actor* newActor)
{
	// TODO: 중복 여부 확인 예외처리 하면 좋음

	if (!newActor)
	{
		return;
	}

	addRequestedActors.emplace_back(newActor);
}

void Level::DestroyActor(Actor* destroyActor)
{
	destroyRequstedActors.emplace_back(destroyActor); // 대기 배열에 추가
}

void Level::ProcessAddAndDestroyActors()
{
	// 삭제할 액터 배열 제외
	for (auto it = actors.begin(); it != actors.end();)
	{
		if ((*it)->IsExpired())
		{
			// erase 함수를 사용하면 iterator가 무효화되기 때문에 
			// 반환되는 값을 저장해야 한다.
			it = actors.erase(it);
			continue;
		}
		++it;
	}

	// 삭제 액터 배열을 순회하면서 delete
	for (auto* actor : destroyRequstedActors)
	{
		// 액터가 그렸던 곳 지우기
		Utils::SetConsolePosition(actor->Position());

		// 콘솔에서 빈문자 출력해서 지우기
		for (int i = 0; i < actor->Width(); ++i)
		{
			printf(" ");
		}

		actor->OnDestroy();

		SafeDelete(actor);
	}

	destroyRequstedActors.clear();

	// 새로운 액터 추가 처리
	for (auto* const actor : addRequestedActors)
	{
 		actors.emplace_back(actor);
		actor->SetOwner(this); // 오너 설정.
	}

	addRequestedActors.clear();
}

const std::vector<Actor*>& Level::GetActors() const
{
	return actors;
}
