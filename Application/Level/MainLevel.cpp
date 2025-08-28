#include "MainLevel.h"

#include "Engine.h"
#include "Actor/AUnit/AUnit.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"

MainLevel::MainLevel()
	: map(Engine::Height(), std::vector<int>(Engine::Width(), 0))
	, dragBox{quadTree}
{
	// aStar
	aStar.SetMap(map);

	AddActor(new AUnit({ 0, 0 }, aStar));
	AddActor(new AUnit({ 1, 5 }, aStar));
	AddActor(new AUnit({ 4, 10 }, aStar));
	AddActor(new AUnit({ 30, 15 }, aStar));
	AddActor(new AUnit({ 36, 25 }, aStar));

	// TODO: 마무리 할 떄쯤 아래 디버그 켜기 함수 지우기
	debug.ToggleDebugMode();
}

MainLevel::~MainLevel()
{
	quadTree.Clear();
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
}

void MainLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 쿼드트리 업데이트
	UpdateQuadTree();

	// 드래그 박스 업데이트
	dragBox.Tick();

	// 틸트'~' 키를 눌렀을 때 디버그 모드를 토글
	if (Input::Get().GetKeyDown(VK_OEM_3))
	{
		debug.ToggleDebugMode();
	}
}

/*
* 랜더 순서
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
}

void MainLevel::UpdateQuadTree()
{
	quadTree.Clear();
	auto actors = GetActors();
	for (const Actor* actor : actors)
	{
		if (actor->As<AUnit>())
		{
			quadTree.Insert((AUnit*)actor);
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
		// 그리드 그리기
		DrawGrids(renderer, 0, 0, Engine::Width() - 1, Engine::Height() - 1);

		// 현재 쿼드 트리 정보 그리기
		quadTree.DrawBounds(renderer);
	}

	// 마우스 위치
	char debugMouse[100];
	sprintf_s(debugMouse, sizeof(debugMouse), "M ( %d , %d )", Input::Get().GetMouseX(), Input::Get().GetMouseY());
	renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, Debug::RenderOrder() + 2);
}

void MainLevel::DrawGrids(Renderer& renderer, int x, int y, int w, int h, int depth)
{
	if (depth > 3)
	{
		return;
	}

	Color color = Color::Intensity;

	int renderOrder = Debug::RenderOrder() - 4;


	// 현재 노드 경계 그리기
	char element[2] = { '.', '\0' };
	// 1. 윗변과 아랫변 그리기 (현재 영역의 너비 사용)
	for (int ix = x; ix < x + w; ++ix)
	{
		renderer.WriteToBuffer({ ix, y }, element, color, renderOrder);
		renderer.WriteToBuffer({ ix, y + h }, element, color, renderOrder);
	}

	// 2. 왼쪽 변과 오른쪽 변 그리기 (현재 영역의 높이 사용)
	for (int iy = y; iy < y + h; ++iy)
	{
		renderer.WriteToBuffer({ x, iy }, element, color, renderOrder);
		renderer.WriteToBuffer({ x + w, iy }, element, color, renderOrder);
	}

	int halfWidth = w / 2;
	int halfHeight = h / 2;

	// 1사분면 (우상단)
	DrawGrids(renderer, x + halfWidth, y, halfWidth, halfHeight, depth + 1);

	// 2사분면 (좌상단)
	DrawGrids(renderer, x, y, halfWidth, halfHeight, depth + 1);

	// 3사분면 (좌하단)
	DrawGrids(renderer, x, y + halfHeight, halfWidth, halfHeight, depth + 1);

	// 4사분면 (우하단)
	DrawGrids(renderer, x + halfWidth, y + halfHeight, halfWidth, halfHeight, depth + 1);
}
