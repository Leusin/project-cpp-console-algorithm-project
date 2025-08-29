#include "AStar.h"

#include <cmath>
#include <algorithm>
#include "ANode.h"
#include "Render/Renderer.h"
#include "Level/Map/Map.h"


AStar::AStar()
	: startNode{ nullptr }, goalNode{ nullptr }
{
}

AStar::~AStar()
{
	// 메모리 해제
	Clear();
}

std::vector<Vector2I> AStar::FindPath(const Vector2I& start, const Vector2I& goal, const Map& map)
{
	if (!map.CanMove(goal))
	{
		return std::vector<Vector2I>();
	}

	// 이전 작업이 있다면 비우기
	Clear();

	startNode = new ANode(start.x, start.y);
	Vector2I clampedGoal = { std::clamp(goal.x, 0, map.Width()), std::clamp(goal.y, 0, map.Height()) };
	goalNode = new ANode(clampedGoal.x, clampedGoal.y);

	allNodes.emplace_back(startNode);
	allNodes.emplace_back(goalNode);

	// 상화좌우, 대각선 방향 및 비용
	Vector2I directions[8] =
	{
		// 상하좌우 (화면 좌표계 기준)
		{0, -1,},  // 상
		{0,  1,},  // 하
		{-1, 0,},  // 좌
		{1,  0,},  // 우

		// 대각선
		{-1, -1}, // 좌상
		{1,  -1}, // 우상
		{-1,  1}, // 좌하
		{1,   1}  // 우하
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
			// 종합 비용(f)이 훨씬 낮은 경우 || 종합 비용(f)이 같지만 추정치 값이 더 높은 경우
			if (node->FCost() < current->FCost() ||
				node->FCost() == current->FCost() && node->hCost < current->hCost)
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
			// 위치 비교
			if (*openList[i] == *current)
			{
				// i 번째 노드 제거
				openList.erase(openList.begin() + i);
				break;
			}
		}

		// 5. 이웃 노드 탐색
		bool isNodeInList = false;
		for (auto& [pos, node] : closedList)
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

		// 닫힌 리스트에 삽입
		closedList[current->position] = current;

		// 이웃 노드 방문
		for (const Vector2I& direction : directions)
		{
			// 다음에 이동할 위치 설정
			int newX = current->position.x + direction.x;
			int newY = current->position.y + direction.y;


			// (옵션) 장애물인지 확인 
			// 여기선 값이 0.0f 이하일 경우 장애물
			if (!map.CanMove({ newX , newY }))
			{
				continue;
			}

			// 대모서리 끼임 문제 해결
			// 이동 방향이 대각선일 경우
			if (direction.x != 0 && direction.y != 0)
			{
				// 둘 다 열려 있어야 대각선 허용
				if (!map.CanMove({ current->position.x + direction.x, current->position.y }) ||
					!map.CanMove({ current->position.x, current->position.y + direction.y }))
				{
					continue;
				}
					
			}

			// 이동 배용
			//float stepCost = 1.0f;
			float stepCost = (direction.x != 0 && direction.y != 0) ? 1.414f : 1.0f;

			// 맵 가중치 적용 (별차이 없는 느낌이다)
			stepCost += stepCost * map.GetWeightMap({ newX , newY});

			// 이미 방문했는지 확인
			float gCost = current->gCost + stepCost;
			if (HasVisited(newX, newY, gCost, current))
			{
				continue;
			}

			// 방문을 위한 노드 생성
			ANode* neighbor = new ANode(newX, newY, current);
			// 메모리 관리를 위한 컨테이너에 등록
			allNodes.emplace_back(neighbor);
			// 비용도 계산
			neighbor->gCost = gCost;

			// 휴리스틱 비용 계산
			neighbor->hCost = CalculateHeuristic(neighbor, goalNode);

			// openListNode 에 이웃 노드가 있는지 검사
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

bool AStar::HasVisited(int x, int y, float gCost, ANode* parent)
{
	// 열린 리스트 또는 닫힌 리스트에 해당 노드가 있으면 방문 한 것으로 판단

	// 1. 열린 리스트에 있는지 검사
	for (size_t i = 0; i < openList.size(); ++i)
	{
		ANode* node = openList[i];
		if (node->position.x == x && node->position.y == y)
		{
			// 이미 더 좋은 경로가 있음
			if (node->gCost <= gCost)
			{
				return true;
			}

			// 더 나은 경로 발견했다면 갱신
			node->gCost = gCost;
			node->parent = parent;
			node->hCost = CalculateHeuristic(node, goalNode);

			return false;
		}
	}

	//2. 닫힌 리스트에 있는지 검사
	auto it = closedList.find({ x, y });
	if (it != closedList.end())
	{
		ANode* node = it->second;

		// 더 좋은 경로가 있다면 닫힌 리스트에서 제거하고 다시 탐색
		if (gCost < node->gCost)
		{
			closedList.erase(it);
			node->gCost = gCost;
			node->parent = parent;
			node->hCost = CalculateHeuristic(node, goalNode);
			openList.emplace_back(node);

			return false;
		}

		// 기존 경로가 더 낫다면 방문 처리
		return true;
	}

	return false;
}

float AStar::CalculateHeuristic(ANode* current, ANode* goal)
{
	Vector2F diff { *current - *goal };

	diff.Abs();

	// 채비쇼프 거리로
	/*
	* 체비쇼프 거리(Chebyshev distance)란
	* - 체스판 거리(chessboard distance) 라고도 불fla
	* - 두 점이 있을 때 각 좌표의 차이 중 더 큰 값을 취함
	*/
	if (diff.x > diff.y)
	{
		return diff.x;
	}
	else
	{
		return diff.y;
	}

	// 휴리스틱 종류
	/*
	// 맨해튼 거리 (직교 전용)
	return (float)(std::abs(diff.x) + std::abs(diff.y));

	// 유클리드 거리 (대각선 허용 시)
	return std::sqrt(diff.x * diff.x + diff.y * diff.y);

	// 채비쇼프Chebyshev 거리 (대각선 허용 + 대각선 비용을 1로 취급할 때)
	return (float)std::max(std::abs(diff.x), std::abs(diff.y));

	// 디아고널(diagonal) 거리 휴리스틱
	return (float)(diff.x + diff.y) + (1.414f - 2.0f) * std::min(dx, dy);
	*/
}