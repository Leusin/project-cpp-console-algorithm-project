#include "MainLevel.h"

#include "Actor/Unit/Unit.h"
#include "Math/Vector2I.h"

// TEST TODO: 테스트 마치고 지울 것
#include "Input.h"
#include <iostream>
#include "Engine.h"

MainLevel::MainLevel()
{
	AddActor(new Unit({0, 0}));
	AddActor(new Unit({1, 5}));
	AddActor(new Unit({4, 10}));
	AddActor(new Unit({30, 15}));
	AddActor(new Unit({36, 25}));
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
		if (actor->As<Unit>())
		{
			quradTree.Insert((Unit*)actor);
		}
	}
}

void MainLevel::Render()
{
	super::Render();

	quradTree.DrawBounds();

	/// 마우스 입력 TEST
	if (Input::Get().GetMouseClick())
	{
		// 마우스 클릭이 발생했을 때
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "Mouse ( %d , %d )", Input::Get().GetMouseX(), Input::Get().GetMouseY());
		Engine::Get().WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, 100);
	}
	if (Input::Get().GetKey('A'))
	{
		// 마우스 클릭이 발생했을 때
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "Pressed  A !!");
		Engine::Get().WriteToBuffer({ 10, 10 }, debugMouse, Color::LightGreen, 100);
	}
}
