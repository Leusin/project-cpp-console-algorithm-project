#include "QuadTree.h"

#include "Bounds.h"
#include "QNode.h"
#include "Core.h"
#include "Engine.h"
#include "Actor/QEntity/QEntity.h"
#include "Render/Renderer.h"
#include "Game/DebugMode.h"

int QuadTree::maxDepth = 3;

QuadTree::QuadTree()
	: root{ nullptr }
{
	CreateRoot();
}

QuadTree::~QuadTree()
{
	Clear();
}

void QuadTree::Insert(QEntity* unit)
{
	CreateRoot();

	root->Insert(unit);
}

void QuadTree::Clear()
{
	SafeDelete(root);
}

bool QuadTree::Query(const QEntity* targetUnit, std::vector<QNode*>& possibleNode)
{
	possibleNode.clear();

	// 겹침 가능성이 있는 영역 확인
	root->Query(targetUnit->GetBounds(), possibleNode);

	// 가능성이 있는 노드에서 실제로 겹치는지 노드 검사
	std::vector<QEntity*> intersects;

	// 겹침 가능성 있는 영역 순회
	for (QNode* node : possibleNode)
	{
		// 각 영역이 가지는 노드 순회
		for (QEntity* point : node->GetPointers())
		{
			// 겹침 검사
			if (point->GetBounds().Intersects(targetUnit->GetBounds()))
			{
				intersects.emplace_back(point);
				continue;
			}
		}
	}

	// 최종 목록 반환.
	// 노트: 함수에서 출력용 배열을 참조로 받으면 최적화 가능

	if (possibleNode.empty())
	{
		return false;
	}

	return true;
}

bool QuadTree::Query(const Bounds& targetBounds, std::vector<class QEntity*>& intersects)
{
	std::vector<QNode*> possibleNode;

	// 겹침 가능성이 있는 영역 확인
	root->Query(targetBounds, possibleNode);

	// 겹침 가능성 있는 영역 순회
	for (QNode* node : possibleNode)
	{
		// 각 영역이 가지는 노드 순회
		for (QEntity* point : node->GetPointers())
		{
			// 겹침 검사
			if (point->GetBounds().Intersects(targetBounds))
			{
				intersects.emplace_back(point);
				continue;
			}
		}
	}

	if (intersects.empty())
	{
		return false;
	}

	return true;
}

void QuadTree::DrawBounds(Renderer& renderer)
{
	DrawGrids(renderer, 0, 0, Engine::Width() - 1, Engine::Height() - 1);

	if (root)
	{
		root->DrawBounds(renderer);
	}
}

void QuadTree::CreateRoot()
{
	if (!root)
	{
		root = new QNode({ 0, 0, Engine::Width() - 1, Engine::Height() -1 });
	}
}

void QuadTree::DrawGrids(Renderer& renderer, int x, int y, int w, int h, int depth)
{
	if (depth > maxDepth)
	{
		return;
	}

	Color color = Color::Intensity;

	int renderOrder = DebugMode::RenderOrder() - 4;

	// 현재 노드 경계 그리기
	char element[2] = { '.', '\0' };
	// 1. 윗변과 아랫변 그리기 (현재 영역의 너비 사용)
	for (int ix = x; ix < x + w; ++ix)
	{
		renderer.WriteToBuffer({ ix, y }, element, color, renderOrder);
		renderer.WriteToBuffer({ ix, y + h }, element, color, renderOrder);
	}

	// 2. 왼쪽 변과 오른쪽 변 그리기 (현재 영역의 높이 사용)
	for (int iy = y; iy < y + h; ++iy)
	{
		renderer.WriteToBuffer({ x, iy }, element, color, renderOrder);
		renderer.WriteToBuffer({ x + w, iy }, element, color, renderOrder);
	}

	int halfWidth = w / 2;
	int halfHeight = h / 2;

	// 1사분면 (우상단)
	DrawGrids(renderer, x + halfWidth, y, halfWidth, halfHeight, depth + 1);

	// 2사분면 (좌상단)
	DrawGrids(renderer, x, y, halfWidth, halfHeight, depth + 1);

	// 3사분면 (좌하단)
	DrawGrids(renderer, x, y + halfHeight, halfWidth, halfHeight, depth + 1);

	// 4사분면 (우하단)
	DrawGrids(renderer, x + halfWidth, y + halfHeight, halfWidth, halfHeight, depth + 1);
}
