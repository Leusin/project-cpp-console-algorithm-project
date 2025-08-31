#pragma once

#include <vector>
#include <queue>
#include "ANode.h"
#include "Math/Vector2I.h"

/// <summary>
/// 목표지점에 갈 수 없으면 가까운 지점까지라도 간다.(약 거리 2)
/// </summary>
class AStar
{
	struct CompareANode
	{
		// 참고: std::priority_queue는 기본이 max-heap
		bool operator()(const class ANode& a, const class ANode& b) const;
	};

public: // RAII

	AStar();
	~AStar();

public: // MESSAGE

	// 경로 탐색
	std::vector<Vector2I> FindPath(const Vector2I& start, const Vector2I& goal, const class Map& map);

	const int& GetCalled()const { return called; }

private: // METHOD

	void Clear();

	// 탐색을 마친 후 경로를 조립해 반환하는 함수
	// 목표 노드에서 부모 노드를 참조해 시작 노드까지 역추적 하면서 경로 구함
	std::vector<Vector2I> ConstructPath(const int& index);

	// 휴리스틱 계산
	inline float CalculateHeuristic(const ANode& current, const ANode& goal) const;

	bool FindNearbyValidSpot(Vector2I& target, const class Map& map);

	// 이차원 좌표를 일차원 배열 인덱스로 변환
	inline int GridIndex(int x, int y, int w) { return y * w + x; }

private: // METHOD

	// 맵 크기
	int width;
	int height;

	// 방문한 모든 노드
	// 닫힌 리스트: 방문된 노드 목록
	std::vector<int> parentIndex;
	std::vector<float> gScore;       // size = width*height, +INF로 초기화
	std::vector<bool> closed;     //  방문 마킹

	// 디버깅용  호출 수 추적
	int called;

	// 검사 방향
	const Vector2I directions[8] =
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
};