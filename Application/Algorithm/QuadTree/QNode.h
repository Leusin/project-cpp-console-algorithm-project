#pragma once

#include "Bounds.h"
#include <vector>

enum class NodeIndex
{
	// 완전히 포함하는 경우
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,

	Straddling, // 두 영역에 겹친 경우
	OutOfArea	// 영역에 벗어난 경우
};

class QNode
{
public: // RAII

	QNode(const Bounds& bounds, int depth = 0);
	~QNode();

public: // GET SET

	// 삽입
	void Insert(class QEntity* node);

	// 질의
	void Query(const Bounds& queryBounds, std::vector<class QNode*>& possibleNodes);

	// 정리
	void Clear();

	void DrawBounds(class Renderer& renderer);

public: // GET SET

	Bounds GetBounds() const { return bounds; }
	const std::vector<class QEntity*>& GetPointers() const { return points; }

	QNode* GetTopLeft() const { return topLeft; }
	QNode* GetTopRight() const { return topRight; }
	QNode* GetBottomLeft() const { return bottomLeft; }
	QNode* GetBottomRight() const { return bottomRight; }

private: // MESSAGE

	// 겹치는지 또는 포함되는지 확인
	NodeIndex TestRegion(const Bounds& bounds);

	// 요청한 Bounds와 겹치는 4분면 목록 반환 함수
	std::vector<NodeIndex>GetQuads(const Bounds& bounds);

	// 4분면 분할 함수
	bool Subdivide();

	// 분할됐는지 확인하는 함수
	bool IsDivided();

private: // FILD

	// 깊이.
	int depth = 0;

	// 영역.
	Bounds bounds;

	// 현재 노드에 포함된 노드 (배열).
	std::vector<class QEntity*> points;

	// 자식 노드
	QNode* topLeft = nullptr;
	QNode* topRight = nullptr;
	QNode* bottomLeft = nullptr;
	QNode* bottomRight = nullptr;
};