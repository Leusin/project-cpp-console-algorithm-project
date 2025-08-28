#include "MainLevel.h"
#include <timeapi.h>

#include "Engine.h"
#include "Actor/AUnit/AUnit.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"
#include "Actor/AUnit/AUnit.h"
#include "QuadTree/QuadTree.h"
#include "AStar/AStar.h"

MainLevel::MainLevel()
	: map(Engine::Height(), std::vector<int>(Engine::Width(), 0))
	, dragBox{ quadTree, selectedUnits }
{
	// aStar
	aStar.SetMap(map);

	AddActor(new AUnit({ 0, 0 }));
	AddActor(new AUnit({ 1, 5 }));
	AddActor(new AUnit({ 4, 10 }));
	AddActor(new AUnit({ 30, 15 }));
	AddActor(new AUnit({ 36, 25 }));

	// TODO: 마무리 할 떄쯤 아래 디버그 켜기 함수 지우기
	debug.ToggleDebugMode();
}

MainLevel::~MainLevel()
{
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
}

void MainLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 드래그 박스 업데이트
	dragBox.Tick();

	// 쿼드트리 업데이트
	UpdateQuadTree();

	// 유닛 선택 해제
	MoveSelectedUnits();

	// 틸트'~' 키를 눌렀을 때 디버그 모드를 토글
	if (Input::Get().GetKeyDown(VK_OEM_3))
	{
		debug.ToggleDebugMode();
	}
}

void MainLevel::SlowTick(float deltaTime)
{

}

/*
* 랜더 순서
* 500: 마우스 포인터
* 300: 드레그
*
* 200: 유닛
*
* 디버깅 랜더는 공통적으로 100 내외
* 102 - 마우스 위치
* 101 - A* 경로
* 100 - 쿼드 트리그리드(depth 3)
* 99 - 쿼드 트리그리드(depth 2)
* 98 - 쿼드 트리그리드(depth 1)
* 97 - 쿼드 트리그리드(depth 0)
* 96 - 기본 그리드
*/
void MainLevel::Draw(Renderer& renderer)
{
	super::Draw(renderer);

	dragBox.Draw(renderer);

	// 디버그 정보 랜더
	DrawDebug(renderer);

	// 마우스 랜더
	if (!DebugMode::IsDebugMode())
	{
		renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, "+", Color::Green, 500);
	}
}

void MainLevel::UpdateQuadTree()
{
	quadTree.Clear();
	auto actors = GetActors();
	for (const Actor* actor : actors)
	{
		if (!actor->IsExpired() && actor->As<AUnit>())
		{
			quadTree.Insert((AUnit*)actor);
		}
	}
}

void MainLevel::DrawDebug(Renderer& renderer)
{
	if (!DebugMode::IsDebugMode())
	{
		return;
	}

	static bool drawGreed = true;
	if (Input::Get().GetKeyDown('1')) { drawGreed = !drawGreed; }
	if (drawGreed)
	{
		// 현재 쿼드 트리 정보 그리기
		quadTree.DrawBounds(renderer);
	}

	// 마우스 위치
	char debugMouse[100];
	sprintf_s(debugMouse, sizeof(debugMouse), "M(%d,%d)", Input::Get().GetMouseX(), Input::Get().GetMouseY());
	renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, DebugMode::RenderOrder() + 2);
}

void MainLevel::MoveSelectedUnits()
{
	if (Input::Get().GetMouseUp(MounseButton::Right))
	{
		if (selectedUnits.empty())
		{
			return;
		}

		int unitCount = (int)selectedUnits.size();

		// 최종 위치
		Vector2I mouseDestination{ Input::Get().GetMouseX(), Input::Get().GetMouseY() };

		// 유닛들을 5x5 그리드로 분산시키기 위한 오프셋 계산
		const int offsetFactor = 1;
		const int gridWidth = 5;

		// 최종 너비 개산. 유닛이 5보다 적으면 n 열로
		int groupWidth = (unitCount < gridWidth) ? unitCount : gridWidth;
		int groupHeight = (unitCount /*+ gridWidth - 1*/) / gridWidth;

		// 최종 너비에 따른 오프셋 적용
		int groupOffsetX = (groupWidth - 1) * offsetFactor / 2;
		int groupOffsetY = (groupHeight - 1) * offsetFactor / 2;

		for (int i = 0; i < unitCount; ++i)
		{
			AUnit* unit = selectedUnits[i];

			// 그리드 위치 계산
			int gridX = i % gridWidth;
			int gridY = i / gridWidth;

			// 최종 목적지에 오프셋 적용
			Vector2I finalDestination = {
				mouseDestination.x + (gridX * offsetFactor) - groupOffsetX,
				mouseDestination.y + (gridY * offsetFactor) - groupOffsetY
			};

			unit->SetMove(finalDestination, aStar);

		}
	}
}
