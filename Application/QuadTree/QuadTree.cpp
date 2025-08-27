#include "QuadTree.h"

#include "Bounds.h"
#include "QNode.h"
#include "Core.h"
#include "Engine.h"
#include "Actor/QEntity/QEntity.h"
#include "Render/Renderer.h"

int QuadTree::maxDepth = 3;
int QuadTree::renderOrder = 0;

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

void QuadTree::DrawBounds(Renderer& renderer)
{
	if (root)
	{
		root->DrawBounds(renderer);
	}
}

void QuadTree::CreateRoot()
{
	if (!root)
	{
		root = new QNode({ 0, 0, Engine::Width(), Engine::Height() });
	}
}
