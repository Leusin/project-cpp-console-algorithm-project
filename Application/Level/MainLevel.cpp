#include "MainLevel.h"
#include <timeapi.h>

#include "Engine.h"
#include "Actor/AUnit/AUnit.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "AStar/AStar.h"
#include "Actor/Territory/Territory.h"

MainLevel::MainLevel()
	: dragBox{ quadTree, selectedUnits }
	, unitFactory{ aStar, map, quadTree }
{
	// 맵 데이터 로드
	map.Initialize();

	// 땅따먹기 영역 만들기
	if (std::vector<Territory*> terrs; map.CreateTerritory(quadTree, terrs))
	{
		for (Territory* terr : terrs)
		{
			AddActor(terr);
		}
		terrs.clear();
	}

	AddActor(unitFactory.CreatUnit({0, 0}));
	AddActor(unitFactory.CreatUnit({1, 0}));
	AddActor(unitFactory.CreatUnit({2, 0}));
	AddActor(unitFactory.CreatUnit({1, 5}, Team::Type::T));
	AddActor(unitFactory.CreatUnit({2, 5}, Team::Type::T));
	AddActor(unitFactory.CreatUnit({3, 5}, Team::Type::T));
	AddActor(unitFactory.CreatUnit({4, 10}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({5, 10}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({6, 10}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({30, 15}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({31, 15}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({32, 15}, Team::Type::Z));
	AddActor(unitFactory.CreatUnit({36, 25}, Team::Type::P));
	AddActor(unitFactory.CreatUnit({37, 25}, Team::Type::P));
	AddActor(unitFactory.CreatUnit({38, 25}, Team::Type::P));

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

	// 유닛 선택 해제
	MoveSelectedUnits();
}

void MainLevel::SlowTick(float deltaTime)
{
	// 쿼드트리 업데이트
	UpdateQuadTree();
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
* 
* 2 - 땅따먹기영역
* 1 - 맵
*/
void MainLevel::Draw(Renderer& renderer)
{
	super::Draw(renderer);

	// 맵
	map.Draw(renderer);

	// 드래그 박스
	dragBox.Draw(renderer);

	// 디버그 정보 랜더
	DrawDebug(renderer);

	// 마우스 랜더
	if (!DebugManage::IsDebugMode())
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
	// 틸트'~' 키를 눌렀을 때 디버그 모드를 토글
	if (Input::Get().GetKeyUp(VK_OEM_3))
	{
		debug.ToggleDebugMode();
	}

	if (!DebugManage::IsDebugMode())
	{
		return;
	}

	// 키 입력 처리
	// 디버그 모드일 경우 모드 인덱스 순서 번호 키누르면 해당 모드로
	for (int i = 0; i < DebugManage::Mode::SIZE; ++i)
	{
		char key = (char)(i + '0');
		if (Input::Get().GetKeyUp(key))
		{
			debug.mode = static_cast<DebugManage::Mode>(i);
		}
	}
	if (Input::Get().GetKeyUp('0' + (char)DebugManage::Mode::SIZE))
	{
		int modeSize = (int)DebugManage::Mode::SIZE;
		debug.mode = static_cast<DebugManage::Mode>(((int)(debug.mode) + 1) % modeSize);
	}

	//
	// 모드와 상관없이 그려지는 것들
	// General Debug Information
	//

	// 마우스 위치
	char debugMouse[100];
	sprintf_s(debugMouse, sizeof(debugMouse), "M(%d,%d)", Input::Get().GetMouseX(), Input::Get().GetMouseY());
	renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, DebugManage::RenderOrder() + 2);

	// 우하단에 쓰여질 설명들
	const static int bufferSize = Engine::Width();
	std::vector<char> debugText(bufferSize);
	int line = Engine::Height();

	int firstLength = sprintf_s(debugText.data(), bufferSize, "[~]ToggleDEBUG/[0]All [1]QT [2]weight [3]next/IDX:%d", (int)debug.mode);
	int firstOffset = Engine::Width() - firstLength;
	renderer.WriteToBuffer({ firstOffset, --line }, debugText.data(), Color::LightGreen, DebugManage::RenderOrder() + 50);

	int secondLength = sprintf_s(debugText.data(), bufferSize, "ManagedQEntity:%d|A*Call:%d", quadTree.GetEntityCount(), aStar.GetCalled());
	int secondOffset = Engine::Width() - secondLength;
	renderer.WriteToBuffer({ secondOffset, --line }, debugText.data(), Color::LightGreen, DebugManage::RenderOrder() + 50);

	//
	// 모드에 따라 그려지는 것들
	//

	switch (debug.mode)
	{
	case DebugManage::Mode::QuadTree:
	{
		// 현재 쿼드 트리 정보 그리기
		quadTree.DrawBounds(renderer);
	}
	break;
	case DebugManage::Mode::Weight:
	{
		// 맵 가중치 데이터
		map.DrawWeight(renderer);
	}
	break;
	case DebugManage::Mode::ALL:
	{
		// 맵 가중치 데이터
		map.DrawWeight(renderer);

		// 현재 쿼드 트리 정보 그리기
		quadTree.DrawBounds(renderer);
	}
	// break; // 일부러 안 넣음
	default:
		break;
	}

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
		int groupHeight = (unitCount) / gridWidth;

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

			unit->SetMove(finalDestination);

		}
	}
}
