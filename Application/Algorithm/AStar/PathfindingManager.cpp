#include "PathfindingManager.h"

PathfindingManager::PathfindingManager()
	: maxReQuestsPerFrame{ 3 }
{
	timer.SetTargetTime(1.f);
}

void PathfindingManager::AddRequest(AUnit* unit, const Vector2I& start, const Vector2I& goal, const Map& map)
{
	// 이미 요청이 존재하는 경우를 대비하여 기존 요청 취소
	CancelRequest(unit);

	PathfindingRequest newRequest = { unit, start, goal, map, false };
	requestQueue.push_back(newRequest);
}

void PathfindingManager::CancelRequest(AUnit* unit)
{
	for (auto& request : requestQueue)
	{
		if (request.unit == unit)
		{
			request.isCancelled = true;
			break;
		}
	}
}

void PathfindingManager::Update(float deltaTime)
{
	timer.Tick(deltaTime);

	int processedCount = 0;
	auto it = requestQueue.begin();

	while (it != requestQueue.end() && processedCount < maxReQuestsPerFrame)
	{
		if (timer.IsTimeout())
		{
			timer.Reset();
			break;
		}

		// 취소된 요청은 처리하지 않고 바로 제거
		if (it->isCancelled)
		{
			it = requestQueue.erase(it);
			continue;
		}

		PathfindingRequest request = *it;

		std::vector<Vector2I> path = aStar.FindPath(request.start, request.goal, request.map);

		it->unit->SetPath(path);
		it = requestQueue.erase(it);

		++processedCount;
	}
}