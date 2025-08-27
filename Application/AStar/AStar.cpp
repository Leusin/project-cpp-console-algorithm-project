#include "AStar.h"

#include <cmath>
#include <algorithm>
#include "ANode.h"

AStar::AStar()
	: startNode{ nullptr }, goalNode{ nullptr }, map{ nullptr }
{
}

AStar::~AStar()
{
	// 메모리 해제
	Clear();
}

std::vector<Vector2I> AStar::FindPath(const Vector2I& start, const Vector2I& goal)
{
	if (!map) // 맵 데이터가 없는 채임
	{
		__debugbreak();
	}

	// 이전 작업이 있다면 비우기
	Clear();

	startNode = new ANode(start.x, start.y);
	Vector2I clampedGoal = { std::clamp(goal.x, 0, (int)(*map)[0].size() - 1), std::clamp(goal.y, 0, (int)(*map).size() - 1) };
	goalNode = new ANode(clampedGoal.x, clampedGoal.y);

	allNodes.emplace_back(startNode);
	allNodes.emplace_back(goalNode);

	// 상화좌우, 대각선 방향 및 비용
	std::vector<Direction> directions =
	{
		// 상하좌우 (화면 좌표계 기준)
		{0, -1, 1.0f},  // 상
		{0,  1, 1.0f},  // 하
		{-1, 0, 1.0f},  // 좌
		{1,  0, 1.0f},  // 우

		// 대각선
		{-1, -1, 1.414f}, // 좌상
		{1,  -1, 1.414f}, // 우상
		{-1,  1, 1.414f}, // 좌하
		{1,   1, 1.414f}  // 우하
	};

	// 1. 초기화
	openList.emplace_back(startNode);

	while (!openList.empty())
	{
		// 2. fCost 가 가장 낮은 노드 선택

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
			return ConstructPath(current);
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

		closedList.emplace_back(current);

		// 이웃 노드 방문
		for (const Direction& direction : directions)
		{
			// 다음에 이동할 위치 설정
			int newX = current->position.x + direction.x;
			int newY = current->position.y + direction.y;

			// 범위 밖인지 확인
			if (!IsInRange(newX, newY))
			{
				continue;
			}

			// (옵션) 장애물인지 확인
			// 여기선 값이 1일 경우 장애물
			if ((*map)[newY][newX] == -1)
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
			allNodes.emplace_back(neighbor);

			neighbor->gCost = current->gCost + direction.cost;

			// 휴리스틱 비용 계산
			neighbor->hCost = CalculateHeuristic(neighbor, goalNode);

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
		}

	} // While END

	return std::vector<Vector2I>();
}

void AStar::Clear()
{
	// 모든 노드 리스트 비우기
	for (ANode* node : allNodes)
	{
		SafeDelete(node);
	}

	allNodes.clear();
	openList.clear();
	closedList.clear();
}

std::vector<Vector2I> AStar::ConstructPath(ANode* node)
{
	std::vector<Vector2I> path;
	ANode* current = node;

	while (current != nullptr)
	{
		path.emplace_back(current->position);
		current = current->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

bool AStar::IsDestination(const ANode* node)
{
	return *node == *goalNode;
}

bool AStar::IsInRange(int x, int y)
{
	if (map->empty())
	{
		return false;
	}

	if (x < 0 || y < 0 || x >= (int)(*map)[0].size() || y >= (int)map->size())
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
