#pragma once

#include "Level/level.h"
#include "QuadTree/QuadTree.h"
#include "AStar/PathfindingManager.h"
#include "DragBox/DragBox.h"
#include "Map/Map.h"
#include "Actor/AUnit/UnitFactory.h"

#ifdef _DEBUG

#include "Utils/DebugManage.h"

#endif

class MainLevel : public Level
{
	RTTI_DECLARATIONS(MainLevel, Level)

public: // RAII

	MainLevel();
	~MainLevel();

public: // EVENT

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void SlowTick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer) override;

private: // METHOD

	// 업데이트
	void UpdateQuadTree();

	// 우클릭 시 선택된 유닛들 이동 처리
	void MoveSelectedUnits();

private: // FILD

	// 마우스 드래그
	DragBox dragBox;

	// 주요 알고리즘 : A*
	PathfindingManager aStar;

	// 주요 알고리즘 : 쿼드 트리
	QuadTree quadTree;

	// A*가 사용할 맵 정보
	Map map;

	// 드래그로 선택된 유닛들
	std::vector<class AUnit*> selectedUnits;

	UnitFactory unitFactory;

#ifdef _DEBUG

private: // DEBUG

	DebugManage debug;
	float debugDeltaTime = 0;
	void DrawDebug(class Renderer& renderer);

#endif
};