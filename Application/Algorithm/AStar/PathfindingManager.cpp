#include "PathfindingManager.h"

PathfindingManager::PathfindingManager()
	: maxReQuestsPerFrame{ 3 }
{
	timer.SetTargetTime(1.f);
}

void PathfindingManager::AddRequest(AUnit* unit, const Vector2I& start, const Vector2I& goal, const Map& map)
{
	if (unit == nullptr || unit->IsExpired())
	{
		return;
	}

	// 이미 요청이 존재하는 경우를 대비하여 기존 요청 취소
	CancelRequest(unit);

	PathfindingRequest newRequest = { unit, start, goal, map, false };
	requestQueue.push_back({ unit, start, goal, map, false });
}

void PathfindingManager::CancelRequest(AUnit* unit)
{
	// unit이 nullptr이면 무시
	if (!unit)
	{
		return;
	}

	for (auto& req : requestQueue)
	{
		if (auto u = req.unit)
		{
			if (u == unit || u->IsExpired())
			{
				req.isCancelled = true;
			}
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
		if (!it->unit || it->isCancelled)
		{
			it = requestQueue.erase(it); // 이미 삭제되었거나 취소된 경우 제거
			continue;
		}

		// 동기적으로 경로 계산
		std::vector<Vector2I> path = std::move(aStar.FindPath(it->start, it->goal, it->map));

		if (it->unit)
		{
			it->unit->SetPath(std::move(path));

		}
		it = requestQueue.erase(it);

		++processedCount;
	}
}