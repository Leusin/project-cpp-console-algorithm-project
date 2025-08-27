#include "MainLevel.h"

#include "Actor/AUnit/AUnit.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"

// TEST TODO: 테스트 마치고 지울 것
#include "Input.h"
#include <iostream>
#include "Engine.h"

MainLevel::MainLevel()
	:map(Engine::Height(), std::vector<int>(Engine::Width(), 0))
{
	// aStar
	aStar.SetMap(map);

	AddActor(new AUnit({0, 0}, aStar));
	AddActor(new AUnit({1, 5}, aStar));
	AddActor(new AUnit({4, 10}, aStar));
	AddActor(new AUnit({30, 15}, aStar));
	AddActor(new AUnit({36, 25}, aStar));
}

MainLevel::~MainLevel()
{
	quradTree.Clear();
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
}

void MainLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	quradTree.Clear();
	auto actors = GetActors();
	for (const Actor* actor : actors)
	{
		if (actor->As<AUnit>())
		{
			quradTree.Insert((AUnit*)actor);
		}
	}
}

void MainLevel::Draw(Renderer& renderer)
{
	super::Draw(renderer);

	quradTree.DrawBounds(renderer);

	/// 마우스 입력 TEST
	if (Input::Get().GetMouse(MounseButton::Left) || Input::Get().GetMouse(MounseButton::Right))
	{
		// 마우스 클릭이 발생했을 때
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "Mouse ( %d , %d )", Input::Get().GetMouseX(), Input::Get().GetMouseY());
		renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, 100);
	}
	if (Input::Get().GetKey('A'))
	{
		// 마우스 클릭이 발생했을 때
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "Pressed  A !!");
		renderer.WriteToBuffer({ 10, 10 }, debugMouse, Color::LightGreen, 100);
	}
}
