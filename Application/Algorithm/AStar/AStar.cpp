#include "AStar.h"

#include <cmath>
#include <algorithm>
#include "Render/Renderer.h"
#include "Level/Map/Map.h"


bool AStar::CompareANode::operator()(const ANode& a, const ANode& b) const
{
	float f1 = a.FCost();
	float f2 = b.FCost();

	// // fCost 같으면 hCost 낮은 게 우선 
	if (f1 == f2)
	{
		return a.hCost > b.hCost;
	}

	return f1 > f2;
}

AStar::AStar()
	: called{ 0 }
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
	int total = width * height;

	// 클로즈 리스트 초기화
	parentIndex.assign(total, -1);
	gScore.assign(total, std::numeric_limits<float>::infinity());
	closed.assign(total, false);

	// 시작노드와 종료 노드 생성
	ANode goalNode{ finalgoal.x, finalgoal.y };

	ANode startNode{ start.x, start.y };
	startNode.gCost = 0;
	startNode.hCost = CalculateHeuristic(startNode, goalNode);

	// 열린 리스트: 방문할 노드 목록
	std::priority_queue<ANode, std::vector<ANode>, CompareANode> openList;
	openList.push(startNode); // 시작 노드 대기열에 넣기

	while (!openList.empty())
	{
		// 가장 비용이 작은 노드 탐색
		const ANode current = openList.top();
		openList.pop();

		// 현재 좌표, 인덱스
		const int currentX = current.position.x;
		const int currentY = current.position.y;
		const int currentIdx = GridIndex(currentX, currentY, width);

		// 목표 확인
		if (current == goalNode)
		{
			// 목표 노드인 경우, 지금까지 경로 반환
			return ConstructPath(currentIdx);
		}

		// 목표가 아닐 경우

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
			if (direction.x && direction.y)
			{
				// 둘 다 열려 있어야 대각선 허용
				if (!map.CanMove({ newX, currentY }) || !map.CanMove({ currentX, newY }))
				{
					continue;
				}
			}

			// 이동 비용
			const float baseCost = (direction.x != 0 && direction.y != 0) ? 1.414f : 1.0f;
			float speedFactor = map.GetWeightMap({ newX, newY }); // 속도 계수
			if (speedFactor <= 0.f)
			{
				continue;
			}
			float stepCost = baseCost / speedFactor; // 속도가 빠르면 비용 낮음

			// 이미 방문했는지 확인
			const int newId = GridIndex(newX, newY, width);
			if (closed[newId])
			{
				continue;
			}
			// 이동 비용이 더 효율적지 확인
			float testGCost = current.gCost + stepCost;
			if (testGCost >= gScore[newId])
			{
				continue;
			}

			// 더 좋은 경로 갱신
			gScore[newId] = testGCost;

			// 방문을 위한 노드 생성
			ANode neighbor{ newX, newY };
			neighbor.gCost = testGCost; // 비용도 계산
			neighbor.hCost = CalculateHeuristic(neighbor, goalNode); // 휴리스틱 비용 계산
			parentIndex[newId] = currentIdx;

			openList.push(neighbor);
		}

	} // While END

	return std::vector<Vector2I>();
}

void AStar::Clear()
{
	width = 0;
	height = 0;

	parentIndex.clear();
	gScore.clear();
	closed.clear();
}

std::vector<Vector2I> AStar::ConstructPath(const int& index)
{
	std::vector<Vector2I> path;
	int current = index;

	while (current != -1)
	{
		// 복호화
		Vector2I parentPosition{ current % width, current / width };
		path.emplace_back(parentPosition);
		current = parentIndex[current];
	}

	std::reverse(path.begin(), path.end());
	return path;
}

float AStar::CalculateHeuristic(const ANode& current, const ANode& goal) const
{
	Vector2F diff{ current - goal };

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

