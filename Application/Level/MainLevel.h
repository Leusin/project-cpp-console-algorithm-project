#pragma once

#include "Level/level.h"
#include "QuadTree/QuadTree.h"
#include "AStar/AStar.h"
#include "DragBox/DragBox.h"
#include "Map/Map.h"
#include "Utils/DebugManage.h"
#include "Actor/AUnit/UnitFactory.h"

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

	// 디버깅 관련
	void DrawDebug(class Renderer& renderer);

	// 우클릭 시 선택된 유닛들 이동 처리
	void MoveSelectedUnits();

private: // FILD

	// 디버그
	DebugManage debug;

	// 마우스 드래그
	DragBox dragBox;

	// 주요 알고리즘 : A*
	AStar aStar;

	// 주요 알고리즘 : 쿼드 트리
	QuadTree quadTree;

	// A*가 사용할 맵 정보
	Map map;

	// 드래그로 선택된 유닛들
	std::vector<class AUnit*> selectedUnits;

	UnitFactory unitFactory;
};