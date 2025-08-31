#include "AStar.h"

#include <cmath>
#include <algorithm>
#include "ANode.h"
#include "Render/Renderer.h"
#include "Level/Map/Map.h"


bool AStar::CompareANode::operator()(const ANode* a, const ANode* b) const
{
	float f1 = a->FCost();
	float f2 = b->FCost();

	// // fCost 같으면 hCost 낮은 게 우선 
	if (f1 == f2)
	{
		return a->hCost > b->hCost;
	}

	return f1 > f2;
}

AStar::AStar()
	: startNode{ nullptr }
	, goalNode{ nullptr }
	, called{ 0 }
	, width{ 0 }
	, height{ 0 }
{
}

AStar::~AStar()
{
	// 메모리 해제
	Clear();
}

std::vector<Vector2I> AStar::FindPath(const Vector2I& start, const Vector2I& goal, const Map& map)
{
	++called; // 디버깅용

	// 이동 가능한 경로인지검사 && 이동 불가하면 가까운 영역을 탐색
	Vector2I finalgoal{ goal };
	if (!map.CanMove(goal) && !FindNearbyValidSpot(finalgoal, map))
	{
		return {};
	}

	// 이전 작업이 있다면 비우기
	Clear();

	// 맵크기 
	width = map.Width();
	height = map.Height();

	// 클로즈 리스트 초기화
	gScore.assign(width * height, std::numeric_limits<float>::infinity());
	closed.assign(width * height, false);

	// 시작노드와 종료 노드 생성
	startNode = new ANode(start.x, start.y);
	goalNode = new ANode(finalgoal.x, finalgoal.y);
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

	// 시작 노드 대기열에 넣기

	// 열린 리스트: 방문할 노드 목록
	openList.push(startNode);

	while (!openList.empty())
	{
		// 가장 비용이 작은 노드 탐색
		ANode* current = openList.top();
		openList.pop();

		// 목표 확인
		if (IsDestination(current))
		{
			// 목표 노드인 경우, 지금까지 경로 반환
			return ConstructPath(current);
		}

		// 목표가 아닐 경우
		
		// 현재 좌표, 인덱스
		const int currentX = current->position.x;
		const int currentY = current->position.y;
		const int currentIdx = GridIndex(currentX, currentY, width);

		// 방문 확인
		closed[currentIdx] = true;

		// 이웃 노드 방문
		for (const Vector2I& direction : directions)
		{
			// 다음에 이동할 위치 설정
			int newX = currentX + direction.x;
			int newY = currentY + direction.y;

			// 장애물 / 맵 가중치가 확인
			if (!map.CanMove({ newX , newY }))
			{
				continue;
			}

			// 이동 방향이 대각선일 경우 (모서리 끼임 문제 대비)
			if (direction.x && direction.y )
			{
				// 둘 다 열려 있어야 대각선 허용
				if (!map.CanMove({ newX, currentY }) || !map.CanMove({ currentX, newY }))
				{
					continue;
				}
			}

			// 이동 비용
			const float baseCost = (direction.x != 0 && direction.y != 0) ? 1.414f : 1.0f;

			// 맵 가중치 적용 (별차이 없는 느낌이다)
			float stepCost = baseCost + baseCost * map.GetWeightMap({ newX , newY });

			//
			// 이미 방문했는지 확인
			//
			const int newId = GridIndex(newX, newY, width);
			if (closed[newId])
			{
				continue;
			}

			float testGCost = current->gCost + stepCost;
			if (testGCost >= gScore[newId])
			{
				continue;
			}

			// 더 좋은 경로 갱신
			gScore[newId] = testGCost;

			// 방문을 위한 노드 생성
			ANode* neighbor = new ANode(newX, newY, current);
			// 메모리 관리를 위한 컨테이너에 등록
			allNodes.emplace_back(neighbor);
			// 비용도 계산
			neighbor->gCost = testGCost;

			// 휴리스틱 비용 계산
			neighbor->hCost = CalculateHeuristic(neighbor, goalNode);

			// 중복 검사 안 하고 그냥 다 push(lazy deletion)
			/*
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
			}
			*/
			openList.push(neighbor);
		}

	} // While END

	return std::vector<Vector2I>();
}

void AStar::Clear()
{

	while (!openList.empty())
	{
		openList.pop();
	}

	width = 0;
	height = 0;

	gScore.clear();
	closed.clear();

	startNode = nullptr;
	goalNode = nullptr;


	// 모든 노드 리스트 비우기
	for (ANode* node : allNodes)
	{
		SafeDelete(node);
	}

	allNodes.clear();
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

float AStar::CalculateHeuristic(ANode* current, ANode* goal)
{
	Vector2F diff{ *current - *goal };

	diff.Abs();

	// 채비쇼프 거리로
	/*
	* 체비쇼프 거리(Chebyshev distance)란
	* - 체스판 거리(chessboard distance) 라고도 불fla
	* - 두 점이 있을 때 각 좌표의 차이 중 더 큰 값을 취함
	*/
	if (diff.x > diff.y)
	{
		return (diff.x + diff.y) + (1.414f - 2.0f) * diff.y;
	}
	else
	{
		return (diff.x + diff.y) + (1.414f - 2.0f) * diff.x;
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

bool AStar::FindNearbyValidSpot(Vector2I& target, const Map& map)
{
	for (int radius = 1; radius <= 5; ++radius)
	{
		// y
		for (int y = target.y - radius; y <= target.y + radius; ++y)
		{
			// x
			for (int x = target.x - radius; x <= target.x + radius; ++x)
			{
				if (x == target.x - radius || x == target.x + radius ||
					y == target.y - radius || y == target.y + radius)
				{
					Vector2I newTarget{ x, y };
					if (map.CanMove(newTarget))
					{
						target = newTarget;

						return true;
					}
				}
			}
		}
	}
	return false;
}

