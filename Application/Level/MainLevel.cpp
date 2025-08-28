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
	, dragBox{ algorithm.quadTree, selectedUnits }
{
	// aStar
	algorithm.aStar.SetMap(map);

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
	MoveUnits();

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
	if (!Debug::IsDebugMode())
	{
		renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, "+", Color::Green, 500);
	}
}

void MainLevel::UpdateQuadTree()
{
	Algorithm::quadTree.Clear();
	auto actors = GetActors();
	for (const Actor* actor : actors)
	{
		if (!actor->IsExpired() && actor->As<AUnit>())
		{
			Algorithm::quadTree.Insert((AUnit*)actor);
		}
	}
}

void MainLevel::DrawDebug(Renderer& renderer)
{
	if (!Debug::IsDebugMode())
	{
		return;
	}

	static bool drawGreed = true;
	if (Input::Get().GetKeyDown('1')) { drawGreed = !drawGreed; }
	if (drawGreed)
	{
		// 현재 쿼드 트리 정보 그리기
		Algorithm::quadTree.DrawBounds(renderer);
	}

	// 마우스 위치
	char debugMouse[100];
	sprintf_s(debugMouse, sizeof(debugMouse), "M(%d,%d)", Input::Get().GetMouseX(), Input::Get().GetMouseY());
	renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, Debug::RenderOrder() + 2);
}

void MainLevel::MoveUnits()
{
	if (Input::Get().GetMouseUp(MounseButton::Right))
	{
		//auto timeStamp1 = timeGetTime();
		if (selectedUnits.empty())
		{
			return;
		}

		//auto outerTimeStamp1 = timeGetTime();
		for (AUnit* unit : selectedUnits)
		{
			//auto timeStamp2 = timeGetTime();

			unit->SetMove({ Input::Get().GetMouseX(), Input::Get().GetMouseY() });

			/* auto timeStamp3 = timeGetTime();

			char buffer[256] = {};
			sprintf_s(buffer, 256, "Period1: %f\n", (float)(timeStamp3 - timeStamp2) / (float)1000.0f);
			OutputDebugStringA(buffer);*/
		}

		/*auto outerTimeStamp2 = timeGetTime();
		char buffer2[256] = {};
		sprintf_s(buffer2, 256, "Total: %f\n", (float)(outerTimeStamp2 - outerTimeStamp1) / (float)1000.0f);
		OutputDebugStringA(buffer2);*/
	}
}
