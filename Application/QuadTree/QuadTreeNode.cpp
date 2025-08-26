#include "QuadTreeNode.h"

#include "QuadTree.h"
#include "Core.h"
#include "Actor/Unit/Unit.h"
#include "Engine.h"

QuadTreeNode::QuadTreeNode(const Bounds& bounds, int depth)
	:bounds{ bounds }, depth{ depth }
{
}

QuadTreeNode::~QuadTreeNode()
{
	Clear();
}

void QuadTreeNode::Insert(Unit* unit)
{
	// 겹치는지 완전히 포함되는지 확인
	NodeIndex result = TestRegion(unit->GetBounds());

	// 여러 영역과 겹쳤는지
	if (result == NodeIndex::Straddling)
	{
		// 하위 노드 검색을 더는 안하고 현재 노드 추가
		points.emplace_back(unit);
	}
	// 겹치지 않은 경우
	else if (result != NodeIndex::OutOfArea)
	{
		// 분할
		if (Subdivide())
		{
			// 어느 영역에 포함되어 있는지 
			if (result == NodeIndex::TopLeft)
			{
				topLeft->Insert(unit);
			}
			if (result == NodeIndex::TopRight)
			{
				topRight->Insert(unit);
			}
			if (result == NodeIndex::BottomLeft)
			{
				bottomLeft->Insert(unit);
			}
			if (result == NodeIndex::BottomRight)
			{
				bottomRight->Insert(unit);
			}
		}
		// 더는 못 나누는 경우 현재 노드에 추가
		else
		{
			points.emplace_back(unit);
		}
	}
}

void QuadTreeNode::Query(const Bounds& queryBounds, std::vector<QuadTreeNode*>& possibleNodes)
{
	// 현재 노드 추가
	possibleNodes.emplace_back(this);

	// 하위 노드 검색
	if (!IsDivided())
	{
		return;
	}

	// 겹치는 4분면 검사
	std::vector<NodeIndex> quads = GetQuads(queryBounds);

	// for 루프로 처리
	for (NodeIndex& index : quads)
	{
		if (index == NodeIndex::TopLeft)
		{
			topLeft->Query(queryBounds, possibleNodes);
		}
		else if (index == NodeIndex::TopRight)
		{
			topRight->Query(queryBounds, possibleNodes);
		}
		else if (index == NodeIndex::BottomLeft)
		{
			bottomLeft->Query(queryBounds, possibleNodes);
		}
		else if (index == NodeIndex::BottomLeft)
		{
			bottomLeft->Query(queryBounds, possibleNodes);
		}

	}
}

// 정리 함수.
void QuadTreeNode::Clear()
{
	// 현재 노드 - 엔진이 알아서 정리함
	points.clear();

	// 분할된 경우 자손 정리.
	if (IsDivided())
	{
		// 자손 정리.
		topLeft->Clear();
		topRight->Clear();
		bottomLeft->Clear();
		bottomRight->Clear();

		// 정리.
		SafeDelete(topLeft);
		SafeDelete(topRight);
		SafeDelete(bottomLeft);
		SafeDelete(bottomRight);
	}
}

void QuadTreeNode::DrawBounds()
{
	// 뎁스에 따 른 색상 변경
	Color color = Color::Intensity;

	switch (depth)
	{
	case 0: // 루트 노드
		color = Color::Intensity;
		break;
	case 1:
		color = Color::Green;
		break;
	case 2:
		color = Color::Yellow;
		break;
	case 3:
		color = Color::White;
		break;
	case 4:
	default:
		color = Color::Intensity;
		break;
	}


	// 현재 노드 경계 그리기
	char hpbarChar[2] = { '#', '\0' };
	for (int ix = 0; ix < bounds.GetWidth(); ++ix)
	{
		// Top
		Engine::Get().WriteToBuffer({ bounds.GetX() + ix, bounds.GetY() }, hpbarChar, color, QuadTree::renderOrder);
		// Bottom
		Engine::Get().WriteToBuffer({ bounds.GetX() + ix, bounds.MaxY() - 1 }, hpbarChar, color, QuadTree::renderOrder);
	}

	for (int iy = 0; iy < bounds.GetHeight(); ++iy)
	{
		// Left
		Engine::Get().WriteToBuffer({ bounds.GetX(), bounds.GetY() + iy }, hpbarChar, color, QuadTree::renderOrder);
		// Right
		Engine::Get().WriteToBuffer({ bounds.MaxX() - 1, bounds.GetY() + iy }, hpbarChar, color, QuadTree::renderOrder);
	}


	// 자손 노드 경계 그리기
	if (IsDivided())
	{
		topLeft->DrawBounds();
		topRight->DrawBounds();
		bottomLeft->DrawBounds();
		bottomRight->DrawBounds();
	}
}

NodeIndex QuadTreeNode::TestRegion(const Bounds& bounds)
{
	// 전달된 bounds와 겹치는 4분면 목록 확인.
	std::vector<NodeIndex> quads = GetQuads(bounds);

	// 겹치는 곳이 없으면 영역 밖으로 표기.
	if (quads.size() == 0)
	{
		return NodeIndex::OutOfArea;
	}

	// 하나만 있는 경우(완전 포함), 해당 4분면 반환.
	if (quads.size() == 1)
	{
		return quads[0];
	}

	// 여러 영역과 겹치는 경우에는 겹침 반환.
	return NodeIndex::Straddling;
}

std::vector<NodeIndex> QuadTreeNode::GetQuads(const Bounds& bounds)
{
	std::vector<NodeIndex> quads;

	// 영역 계산에 필요한 변수.
	int x = this->bounds.GetX();
	int y = this->bounds.GetY();
	int halfWidth = this->bounds.GetWidth() / 2;
	int halfHeight = this->bounds.GetHeight() / 2;
	int centerX = x + halfWidth;
	int centerY = y + halfHeight;

	// 왼쪽 영역과 겹쳤는지 확인.
	bool left = bounds.GetX() < centerX && bounds.MaxX() >= x;

	// 오른쪽 영역과 겹쳤는지 확인.
	bool right = bounds.MaxX() >= centerX && bounds.GetX() < this->bounds.MaxX();

	// 위쪽 영역과 겹쳤는지 확인.
	bool top = bounds.GetY() < centerY && bounds.MaxY() >= y;

	// 아래쪽 영역과 겹쳤는지 확인.
	bool bottom = bounds.MaxY() >= centerY && bounds.GetY() < this->bounds.MaxY();

	if (top && left)
	{
		quads.emplace_back(NodeIndex::TopLeft);
	}
	if (top && right)
	{
		quads.emplace_back(NodeIndex::TopRight);
	}
	if (bottom && left)
	{
		quads.emplace_back(NodeIndex::BottomLeft);
	}
	if (bottom && right)
	{
		quads.emplace_back(NodeIndex::BottomRight);
	}

	return quads;
}

bool QuadTreeNode::Subdivide()
{
	// 최대 깊이에 도달했는지
	// 도달했다면 더는 분할 안 함
	// 최대 depth 값을 상수로 둠
	if (depth == QuadTree::maxDepth)
	{
		return false;
	}

	// 아직 분할 안되었다면 분할
	if (!IsDivided())
	{
		// 영역 나누기

		// 영역 계산을 위한 변수
		int x = bounds.GetX();
		int y = bounds.GetY();
		int halfWidth = bounds.GetWidth() / 2;
		int halfHeight = bounds.GetHeight() / 2;

		// 분할 4분면 객체 생성
		// 4분면 분할 객체 생성
		/*
		+------------+------------+
		|    0 (TL)  |    1 (TR)  |
		|            |            |
		+------------+------------+
		|    2 (BL)  |    3 (BR)  |
		|            |            |
		+------------+------------+
		*/
		topLeft = new QuadTreeNode(Bounds(x, y, halfWidth, halfHeight), depth + 1);
		topRight = new QuadTreeNode(Bounds(x + halfWidth, y, halfWidth, halfHeight), depth + 1);
		bottomLeft = new QuadTreeNode(Bounds(x, y + halfHeight, halfWidth, halfHeight), depth + 1);
		bottomRight = new QuadTreeNode(Bounds(x + halfWidth, y + halfHeight, halfWidth, halfHeight), depth + 1);
	}

	return true;
}

bool QuadTreeNode::IsDivided()
{
	// 하나라도 null이 아니면 분활 완료
	// 자손 노드중 하나가 모두 null이라면 모두 null
	return topLeft != nullptr;
}
