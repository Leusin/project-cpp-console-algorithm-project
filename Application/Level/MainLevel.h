#pragma once

#include "Level/level.h"
#include "QuadTree/QuadTree.h"
#include "AStar/AStar.h"
#include "Debug/Debug.h"

class MainLevel : public Level
{
	RTTI_DECLARATIONS(MainLevel, Level)

public: // RAII

	MainLevel();
	~MainLevel();

public: // EVENT

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer) override;

private: // METHOD
	
	// 업데이트
	void UpdateQuadTree();

	// 디버깅 관련
	void DrawDebug(class Renderer& renderer);
	void DrawGrids(class Renderer& renderer, int x, int y, int w, int h, int depth = 0);

private: // FILD

	// 핵심 알고리즘: 쿼드 트리
	QuadTree quadTree;

	// 핵심 알고리즘: A*
	AStar aStar;

	// A*가 사용할 맵 정보
	const std::vector<std::vector<int>> map;

	// 디버그
	Debug debug;
};