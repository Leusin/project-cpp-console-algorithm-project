#include "MainLevel.h"
#include <timeapi.h>

#include "Engine.h"
#include "Actor/AUnit/AUnit.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "Actor/Territory/Territory.h"
#include "Actor/Territory/SpawnPool.h"

MainLevel::MainLevel()
	: dragBox{ quadTree, selectedUnits }
	, unitFactory{ aStar, map, quadTree }
{
	// 맵 데이터 로드
	map.Initialize();

	// 땅따먹기 영역 만들기
	if (std::vector<Territory*> terrs; map.CreateTerritory(quadTree, unitFactory, terrs))
	{
		for (Territory* terr : terrs)
		{
			AddActor(terr);
		}
		terrs.clear();
	}

	// 스폰 영역 만들기
	if (std::vector<SpawnPool*> pools; map.CreateSpawnPool(quadTree, unitFactory, pools))
	{
		for (SpawnPool* pool : pools)
		{
			AddActor(pool);
		}
		pools.clear();
	}

	// TODO: 마무리 할 떄쯤 아래 디버그 켜기 함수 지우기
	//debug.ToggleDebugMode();
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

	// 모든 유닛 선택 (Ctrl+A)
	if (Input::Get().GetKey(VK_CONTROL) && Input::Get().GetKeyDown('A'))
	{
		selectedUnits.clear();
		const auto& actors = GetActors();
		for (Actor* actor : actors)
		{
			if (!actor->IsExpired())
			{
				if (AUnit* unit = actor->As<AUnit>())
				{
					unit->SetIsSelected(true);
					selectedUnits.push_back(const_cast<AUnit*>(unit));
				}
			}
		}
	}

	// 유닛 선택 해제
	MoveSelectedUnits();

	// 쿼드트리 업데이트
	UpdateQuadTree();

#ifdef _DEBUG

	debugDeltaTime = deltaTime;

#endif
}

void MainLevel::SlowTick(float deltaTime)
{
	// 길찾기 요청 처리
	aStar.Update(deltaTime);

#ifdef _DEBUG

	debugSlowDeltaTime = deltaTime;

#endif
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

	// 마우스 랜더
	renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, "+", Color::Green, 500);

#ifdef _DEBUG

	// 디버그 정보 랜더
	DrawDebug(renderer);

#endif
}

void MainLevel::OnActorDestroyed(Actor* actor) 
{
	if (auto unit = dynamic_cast<AUnit*>(actor)) 
	{
		selectedUnits.erase(
			std::remove(selectedUnits.begin(), selectedUnits.end(), unit),
			selectedUnits.end()
		);
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

void MainLevel::MoveSelectedUnits()
{
	if (Input::Get().GetMouseUp(MounseButton::Right))
	{
		// 없으면 예외
		if (selectedUnits.empty())
		{
			return;
		}

		// 유효하지 않는지 검사
		auto condition = [](const AUnit* unit) { return unit == nullptr || unit->IsExpired(); };

		// 유효하지 않은 포인터 제거
		selectedUnits.erase(std::remove_if(selectedUnits.begin(), selectedUnits.end(), condition), selectedUnits.end());


		int unitCount = (int)selectedUnits.size();

		// 최종 위치
		Vector2I mouseDestination{ Input::Get().GetMouseX(), Input::Get().GetMouseY() };

		// 유닛들을 n열로 분산시키기 위한 오프셋 계산
		int gridWidth = static_cast<int>(ceil(sqrt(unitCount))); // 루트 한 다음에 올림함

		// 최종 너비 개산. 유닛이 5보다 적으면 n 열로
		int groupWidth = (unitCount < gridWidth) ? unitCount : gridWidth;
		int groupHeight = (unitCount) / gridWidth;

		// 최종 너비에 따른 오프셋 적용
		int groupOffsetX = (groupWidth - 1) / 2;
		int groupOffsetY = (groupHeight - 1) / 2;

		int i = 0;
		for (AUnit* unit : selectedUnits) // 인덱스 대신 범위 기반 for 루프 사용
		{
			// 삭제된 경우 다시 검사
			if (unit == nullptr || unit->IsExpired())
			{
				continue;
			}

			// 그리드 위치 계산
			int gridX = i % gridWidth;
			int gridY = i / gridWidth;

			// 최종 목적지에 오프셋 적용
			Vector2I finalDestination =
			{
				mouseDestination.x + (gridX)-groupOffsetX,
				mouseDestination.y + (gridY)-groupOffsetY
			};

			unit->OnCommandToMove(finalDestination);

			++i;
		}
	}
}

#ifdef _DEBUG

void MainLevel::DrawDebug(Renderer& renderer)
{
	// 틸트'~' 키를 눌렀을 때 디버그 모드를 토글
	if (Input::Get().GetKeyUp(VK_OEM_3))
	{
		debug.ToggleDebugMode();
	}

	if (DebugManage::GetMode() == DebugManage::Mode::NONE)
	{
		return;
	}

	// 키 입력 처리
	// 디버그 모드일 경우 모드 인덱스 순서 번호 키누르면 해당 모드로
	int debugIndexSize = (int)DebugManage::Mode::NONE;
	for (int i = 0; i < debugIndexSize; ++i)
	{
		char key = (char)(i + '0');
		if (Input::Get().GetKeyUp(key))
		{
			DebugManage::Mode next = static_cast<DebugManage::Mode>(i);
			DebugManage::SetMode(next);
		}
	}
	if (Input::Get().GetKeyUp('0' + (char)debugIndexSize))
	{
		int modeSize = debugIndexSize;
		DebugManage::Mode next = static_cast<DebugManage::Mode>(((int)(debug.mode) + 1) % modeSize);
		DebugManage::SetMode(next);
	}

	//
	// 모드와 상관없이 우하단에 쓰여질 설명들
	// General Debug Information
	//

	const static int bufferSize = Engine::Width();
	std::vector<char> debugText(bufferSize);
	int line = Engine::Height();

	int firstLength = sprintf_s(debugText.data(), bufferSize, "[~]ToggleDEBUG/[0]All [1]QT [2]weight [3]Pos [4]Path [5]next/IDX:%d", (int)debug.mode);
	int firstOffset = Engine::Width() - firstLength;
	renderer.WriteToBuffer({ firstOffset, --line }, debugText.data(), Color::LightGreen, DebugManage::RenderOrder() + 50);

	int secondLength = sprintf_s(debugText.data(), bufferSize, "AUnit:(%d/%d)|A*Request:%d|A*Call:%d", AUnit::GetCount(), AUnit::GetMaxCount(), aStar.GetCurrentReQuest(), aStar.GetCalled());
	int secondOffset = Engine::Width() - secondLength;
	renderer.WriteToBuffer({ secondOffset, --line }, debugText.data(), Color::LightGreen, DebugManage::RenderOrder() + 50);

	int thirdLength = sprintf_s(debugText.data(), bufferSize, "FPS:%f/%f|WH:%d/%d", (1.f / debugDeltaTime), (1.f / debugSlowDeltaTime), Engine::Width(), Engine::Height());
	int thirdOffset = Engine::Width() - thirdLength;
	renderer.WriteToBuffer({ thirdOffset, --line }, debugText.data(), Color::LightGreen, DebugManage::RenderOrder() + 50);

	//
	// 모드에 따라 그려지는 것들
	//

	switch (DebugManage::GetMode())
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
	case DebugManage::Mode::Position:
	{
		// 마우스 위치
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "M(%d,%d)", Input::Get().GetMouseX(), Input::Get().GetMouseY());
		renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, DebugManage::RenderOrder() + 2);
	}
	break;
	case DebugManage::Mode::Path:
	{

	}
	break;
	case DebugManage::Mode::ALL:
	{
		// 맵 가중치 데이터
		map.DrawWeight(renderer);

		// 현재 쿼드 트리 정보 그리기
		quadTree.DrawBounds(renderer);

		// 마우스 위치
		char debugMouse[100];
		sprintf_s(debugMouse, sizeof(debugMouse), "M(%d,%d)", Input::Get().GetMouseX(), Input::Get().GetMouseY());
		renderer.WriteToBuffer({ Input::Get().GetMouseX(), Input::Get().GetMouseY() }, debugMouse, Color::LightGreen, DebugManage::RenderOrder() + 2);
	}
	// break; // 일부러 안 넣음
	default:
		break;
	}

}

#endif

