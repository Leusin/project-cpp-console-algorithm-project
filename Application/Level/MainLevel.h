#pragma once

#include "Level/level.h"

#include "Game/Debug.h"
#include "DragBox/DragBox.h"
#include "Algorithm.h"

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
	void MoveUnits();

private: // FILD

	// 디버그
	Debug debug;

	// 마우스 드래그
	DragBox dragBox;

	// 주요 알고리즘
	Algorithm algorithm;

	// A*가 사용할 맵 정보
	const std::vector<std::vector<int>> map;

	// 드래그로 선택된 유닛들
	std::vector<class AUnit*> selectedUnits;
};