#pragma once

#include <vector>
#include "Bounds.h"

/*
* 쿼드 트리 클래스
*/
class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	// 노드 추가하는 함수
	void Insert(class Unit* node);

	void Clear();

	// 전달한 노드와 겹치는 노드를 반환 하는 질의 함수
	bool Query(const class Unit* queryNode, std::vector<class QuadTreeNode*>& possibleNode);

	void DrawBounds();

public:

	static int maxDepth;

private: // METHOD
	void CreateRoot();

private:
	class QuadTreeNode* root = nullptr;
};