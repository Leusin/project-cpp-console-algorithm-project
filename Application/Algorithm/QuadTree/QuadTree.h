#pragma once

#include <vector>
#include "Bounds.h"
#include "Math/Color.h"

/*
* 쿼드 트리 클래스
*/
class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	// 노드 추가하는 함수
	void Insert(class QEntity* node);

	void Clear();

	// 전달한 노드와 겹치는 노드를 반환 하는 질의 함수
	bool Query(const class QEntity* queryTarget, std::vector<class QNode*>& possibleNode);
	bool Query(const class Bounds& targetBounds, std::vector<class QEntity*>& intersects);

	void DrawBounds(class Renderer& renderer);

	const int& GetEntityCount() { return managedEntityCount; }

public:
	static int maxDepth;

private: // METHOD

	// 엔진에서 너비 높이를 가져와 루트 생성
	void CreateRoot();

	// 디버깅을 위한 
	void DrawGrids(class Renderer& renderer, int x, int y, int w, int h, int depth = 0);

private:
	class QNode* root = nullptr;

	int managedEntityCount;
};