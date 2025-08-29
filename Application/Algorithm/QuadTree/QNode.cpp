#include "QNode.h"

#include "QuadTree.h"
#include "Core.h"
#include "Actor/QEntity/QEntity.h"
#include "Render/Renderer.h"
#include "Game/DebugManage.h"

QNode::QNode(const Bounds& bounds, int depth)
	:bounds{ bounds }, depth{ depth }
{
}

QNode::~QNode()
{
	Clear();
}

void QNode::Insert(QEntity* unit)
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

void QNode::Query(const Bounds& queryBounds, std::vector<QNode*>& possibleNodes)
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
		else if (index == NodeIndex::BottomRight)
		{
			bottomRight->Query(queryBounds, possibleNodes);
		}
	}
}

// 정리 함수.
void QNode::Clear()
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

void QNode::DrawBounds(Renderer& renderer)
{
	int x = bounds.GetX();
	int y = bounds.GetY();
	int maxX= bounds.MaxX();
	int maxY = bounds.MaxY();

	Color color = Color::White;

	switch (depth)
	{
	case 0: { color = Color::LightWhite; } break;
	case 1: { color = Color::LightCyan; } break;
	case 2: { color = Color::LightMagenta; } break;
	case 3: { color = Color::LightYellow; } break;
	default: { color = Color::Intensity; } break;
	}


	int renderOrder = DebugManage::RenderOrder() - QuadTree::maxDepth + depth;

	// 현재 노드 경계 그리기

	// 꼭짓점 그리기
	renderer.WriteToBuffer({ x, y }, "+", color, renderOrder); // Top-left
	renderer.WriteToBuffer({ maxX, y }, "+", color, renderOrder); // Top-right
	renderer.WriteToBuffer({ x, maxY }, "+", color, renderOrder); // Bottom-left
	renderer.WriteToBuffer({ maxX , maxY }, "+", color, renderOrder); // Bottom-right

	char buffer[2];
	sprintf_s(buffer, sizeof(buffer), "%d", depth);
	renderer.WriteToBuffer({ x + 1 , y + 1 }, buffer, color, renderOrder); // Bottom-right

	for (int ix = x + 1; ix < maxX; ++ix)
	{
		// Top
		renderer.WriteToBuffer({ ix, y }, "-", color, renderOrder);
		// Bottom
		renderer.WriteToBuffer({ ix, maxY }, "-", color, renderOrder);
	}

	for (int iy = y + 1; iy < maxY; ++iy)
	{
		// Left
		renderer.WriteToBuffer({ x, iy }, "|", color, renderOrder);
		// Right
		renderer.WriteToBuffer({ maxX, iy }, "|", color, renderOrder);
	}

	// 자손 노드 경계 그리기
	if (IsDivided())
	{
		topLeft->DrawBounds(renderer);
		topRight->DrawBounds(renderer);
		bottomLeft->DrawBounds(renderer);
		bottomRight->DrawBounds(renderer);
	}
}

NodeIndex QNode::TestRegion(const Bounds& bounds)
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

std::vector<NodeIndex> QNode::GetQuads(const Bounds& bounds)
{
	std::vector<NodeIndex> quads;

	// 영역 계산에 필요한 변수.
	int x = this->bounds.GetX();
	int y = this->bounds.GetY();
	int halfWidth = this->bounds.GetWidth() / 2;
	int halfHeight = this->bounds.GetHeight() / 2;
	int centerX = x + halfWidth;
	int centerY = y + halfHeight;

	/*
	// 왼쪽 영역과 겹쳤는지 확인.
	bool left = bounds.GetX() < centerX && bounds.MaxX() >= x;

	// 오른쪽 영역과 겹쳤는지 확인.
	bool right = bounds.MaxX() >= centerX && bounds.GetX() < this->bounds.MaxX();

	// 위쪽 영역과 겹쳤는지 확인.
	bool top = bounds.GetY() < centerY && bounds.MaxY() >= y;

	// 아래쪽 영역과 겹쳤는지 확인.
	bool bottom = bounds.MaxY() >= centerY && bounds.GetY() < this->bounds.MaxY();
	*/

	bool left = bounds.GetX() <= centerX;
	bool right = bounds.MaxX() >= centerX;
	bool top = bounds.GetY() <= centerY;
	bool bottom = bounds.MaxY() >= centerY;

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

bool QNode::Subdivide()
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
		// 영역 계산을 위한 변수
		int x = bounds.GetX();
		int y = bounds.GetY();
		int width = bounds.GetWidth();
		int height = bounds.GetHeight();
		int halfWidth = width / 2;
		int halfHeight = height / 2;

		// 현재 영역이 정수로만 처리되어서 분할 시 경계선이 어긋날 수 있어서 주의해야 한다
		// ex. 너비가 101인 영역을 이등분하면 50/51, 인 영역이 나와야하지만 처리를 안하면 50/50으로 됨

		// 분할 4분면 객체 생성
		// 4분면 분할 객체 생성
		/*							정수 나누셈에 유의!!
		+------------+-------------+ 
		|    0 (TL)  |    1 (TR)   | 1 TR, 3 BR
		|            |             | 너비: width - halfWidth
		+------------+-------------+
		|    2 (BL)  |    3 (BR)   |
		|            |             | 2 BL, 3 BR
		|            |             | 높이: height - halfHeight
		+------------+-------------+
		*/

		// 0
		topLeft = new QNode(Bounds(x, y, halfWidth, halfHeight), depth + 1);
		// 1
		topRight = new QNode(Bounds(x + halfWidth, y, width - halfWidth, halfHeight), depth + 1);
		// 2
		bottomLeft = new QNode(Bounds(x, y + halfHeight, halfWidth, height - halfHeight), depth + 1);
		// 3
		bottomRight = new QNode(Bounds(x + halfWidth, y + halfHeight, width - halfWidth, height - halfHeight), depth + 1);
	}

	return true;
}

bool QNode::IsDivided()
{
	// 하나라도 null이 아니면 분활 완료
	// 자손 노드중 하나가 모두 null이라면 모두 null
	return topLeft != nullptr;
}
