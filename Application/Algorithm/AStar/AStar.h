#pragma once

#include <vector>
#include <unordered_map>
#include "Math/Vector2I.h"

/// <summary>
/// 목표지점에 갈 수 없으면 가까운 지점까지라도 간다.(약 거리 2)
/// </summary>
class AStar
{
public: // RAII

	AStar();
	~AStar();

public: // MESSAGE

	// 경로 탐색
	std::vector<Vector2I> FindPath(const Vector2I& start, const Vector2I& goal, const class Map& map);

private: // METHOD

	void Clear();

	// 탐색을 마친 후 경로를 조립해 반환하는 함수
	// 목표 노드에서 부모 노드를 참조해 시작 노드까지 역추적 하면서 경로 구함
	std::vector<Vector2I> ConstructPath(class ANode* node);

	// 탐색하려는 노드가 목표 노드인지 검사
	bool IsDestination(const class ANode* node);

	// 이미 방문했는지 확인
	bool HasVisited(int x, int y, float gCost, class ANode* parent);

	// 휴리스틱 계산
	float CalculateHeuristic(class ANode* current, class ANode* goal);

	bool FindNearbyValidSpot(Vector2I& target, const class Map& map);

private: // METHOD

	// 열린 리스트: 방문할 노드 목록
	std::vector<class ANode*> openList;

	// 닫힌 리스트: 방문된 노드 목록
	std::unordered_map<Vector2I, class ANode*, Vector2IHash> closedList;

	// 메모리 관리를 위해 모든 노드를 저장하는
	std::vector<class ANode*> allNodes;

	// 시작
	class ANode* startNode;

	// 목표
	class ANode* goalNode;
};