#pragma once

#include <list>
#include "Math/Vector2I.h"
#include "Actor/AUnit/AUnit.h"
#include "Level/Map/Map.h"
#include "AStar.h"

class PathfindingManager 
{
    struct PathfindingRequest
    {
        AUnit* unit;
        Vector2I start;
        Vector2I goal;
        const Map& map;
        bool isCancelled = false; // 취소 플래그
    };

public: // RAII

    PathfindingManager();

public: // MESSAGE

    void AddRequest(AUnit* unit, const Vector2I& start, const Vector2I& goal, const Map& map);
    void CancelRequest(AUnit* unit);
    void Update();

public: // GET SET

    const int& GetCalled()const { return aStar.GetCalled(); }
    const int GetCurrentReQuest()const { return (int)requestQueue.size(); }

private:
    std::list<PathfindingRequest> requestQueue;

    const int maxReQuestsPerFrame;

    AStar aStar;
};