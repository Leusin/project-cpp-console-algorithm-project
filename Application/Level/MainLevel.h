#pragma once

#include "Level/level.h"
#include "QuadTree/QuadTree.h"
#include "AStar/AStar.h"

class MainLevel : public Level
{
	RTTI_DECLARATIONS(MainLevel, Level)

public: // RAII

	MainLevel();
	~MainLevel();

public: // 

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw(class Renderer& renderer) override;

private: // FILD

	QuadTree quradTree;
	AStar aStar;

	// 맵 정보
	const std::vector<std::vector<int>> map;
};