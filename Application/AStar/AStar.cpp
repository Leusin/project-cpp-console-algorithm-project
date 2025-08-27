#include "AStar.h"

#include <cmath>
#include "Engine.h"
#include "ANode.h"

AStar::AStar()
	: startNode{ nullptr }, goalNode{ nullptr }
{
}

AStar::~AStar()
{
	// 메모리 해제
	Clear();
}

std::vector<class ANode*> AStar::FindPath(const Vector2I& start, const Vector2I& goal, std::vector<std::vector<int>>& grid)
{
	// 이전 작업이 있다면 비우기
	Clear();

	// TODO 좌표와 댜략적인 정보만 받고 생성하도록 만들기
	startNode = new ANode(start.x, start.y);
	goalNode = new ANode(goal.x, goal.y);

	// 상화좌우, 대각선 방향 및 비용
	std::vector<Direction> directions =
	{
		// 하상우좌 (화면 좌표계 기준)
		{0, 1, 1.0f}, {0, -1, 1.0f}, {1, 0, 1.0f}, {-1, 0, 1.0f},
		// 대각선
		{1, 1, 1.414f}, {1, -1, 1.414f}, {1, -1, 1.414f}, {-1, -1, 1.414f}
	};

	// 1. 초기화
	openList.emplace_back(startNode);

	while (!openList.empty())
	{
		//2. fCost 가 가장 낮은 노드 선택

		// 가장 비용이 작은 노드 탐색
		ANode* current = openList[0];

		for (ANode* node : openList)
		{
			if (node->FCost() < current->FCost())
			{
				current = node;
			}
		}

		// 3. 목표 확인
		if (IsDestination(current))
		{
			// 목표 노드인 경우, 지금까지 경로 반환
			return ConstructPath();
		}

		// 4. 닫힌 리스트로 이동 (열린 리스트에선 제거)
		for (size_t i = 0; i < openList.size(); ++i)
		{
			if (*openList[i] == *current)
			{
				// i 번째 노드 제거
				openList.erase(openList.begin() + i);
				break;
			}
		}

		// 5. 이웃 노드 탐색
		bool isNodeInList = false;
		for (ANode* node : closedList)
		{
			if (*node == *current)
			{
				isNodeInList = true;
				break;
			}
		}

		// 방문했으면 아래 단계 건너뜀
		if (isNodeInList)
		{
			continue;
		}

		closedList.emplace_back(startNode);

		// 이웃 노드 방문
		for (const Direction& direction : directions)
		{
			// 다음에 이동할 위치 설정
			int newX = current->position.x + direction.x;
			int newY = current->position.y + direction.y;

			// 그리드 밖인지 확인
			if (!IsInRange(newX, newY))
			{
				continue;
			}

			// (옵션) 장애물인지 확인
			// 여기선 값이 1일 경우 장애물
			if (grid[newY][newX] == 1)
			{
				continue;
			}

			// 이미 방문했는지 확인
			float gCost = current->gCost + direction.cost;
			if (HasVisited(newX, newY, gCost))
			{
				continue;
			}

			// 방문을 위한 노드 생성
			ANode* neighbor = new ANode(newX, newY, current);
			neighbor->gCost = current->gCost + direction.cost;

			// 휴리스틱 비용 계산
			neighbor->hCost = CalculateHeuristic(current, goalNode);

			ANode* openListNode = nullptr;
			for (ANode* node : openList)
			{
				if (*node == *neighbor)
				{
					openListNode = node;
					break;
				}
			}

			// 노드가 목록에 없거나 비용이 싸면, 새 노드 추가.
			if (openListNode == nullptr
				|| openListNode->gCost > neighbor->gCost
				|| openListNode->FCost() > neighbor->FCost())
			{
				openList.emplace_back(neighbor);
			}
			else
			{
				SafeDelete(neighbor);
			}
		}

	} // While END

	// 경로를 찾지 못했음
	SafeDelete(startNode);
	SafeDelete(goalNode);

	return std::vector<class ANode*>();
}

void AStar::Clear()
{
	// 오픈 리스트 비우기
	for (ANode* node : openList)
	{
		SafeDelete(node);
	}

	openList.clear();

	// 클로즈 리스트 비우기
	for (ANode* node : closedList)
	{
		SafeDelete(node);
	}

	closedList.clear();
}

std::vector<ANode*> AStar::ConstructPath()
{
	std::vector<ANode*> path;
	ANode* current = goalNode;

	while (current != nullptr)
	{
		path.emplace_back(current);
		current = current->parent;
	}

	std::reverse(path.begin(), path.end());

	// 참조중인 노드 정리. 할당해제는 내보내지는 벡터에게 위임.
	startNode = nullptr;
	goalNode = nullptr;

	return path;
}

bool AStar::IsDestination(const ANode* node)
{
	return *node == *goalNode;
}

bool AStar::IsInRange(int x, int y)
{
	if (x < 0 || y < 0 || x >= Engine::Width()|| y >= Engine::Height())
	{
		return false;
	}

	return true;
}

bool AStar::HasVisited(int x, int y, float gCost)
{
	// 열린 리스트 또는 닫힌 리스트에 해당 노드가 있으면 방문 한 것으로 판단

	// 열린 리스트에 있는지 검사
	for (size_t i = 0; i < openList.size(); ++i)
	{
		ANode* node = openList[i];
		if (node->position.x == x && node->position.y == y)
		{
			// 위치는 같으나 비용이 더 높은 경우
			if (node->gCost < gCost)
			{
				return true;
			}
			// 위치는 같으나 비용이 작다면 
			else if (node->gCost > gCost)
			{
				openList.erase(openList.begin() + i);
				SafeDelete(node);
			}
		}
	}

	// 닫힌 리스트에 있는지 검사
	for (size_t i = 0; i < closedList.size(); ++i)
	{
		ANode* node = closedList[i];
		if (node->position.x == x && node->position.y == y)
		{
			// 위치는 같으나 비용이 더 높은 경우
			if (node->gCost < gCost)
			{
				return true;
			}
			// 위치는 같으나 비용이 작다면 
			else if (node->gCost > gCost)
			{
				closedList.erase(closedList.begin() + i);
				SafeDelete(node);
			}
		}
	}

	return false;
}

float AStar::CalculateHeuristic(ANode* current, ANode* goal)
{
	Vector2I diff = *current - *goal;

	// 유클리드로 
	return (float)std::sqrt(diff.x * diff.x + diff.y * diff.y);
}
