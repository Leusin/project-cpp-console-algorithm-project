#include "SpawnPool.h"

#include "Level/Level.h"
#include "Level/Map/Map.h"
#include "Render/Renderer.h"
#include "Actor/AUnit/Team.h"
#include "QuadTree/QuadTree.h"
#include "Actor/AUnit/AUnit.h"
#include "Actor/AUnit/UnitFactory.h"


int SpawnPool::count = 0;

SpawnPool::SpawnPool(int id, Vector2I position, Vector2I size, QuadTree& qTree, Map& map, const UnitFactory& factory)
	: Territory(id, position, size, qTree, (Team::Type)(count % (int)Team::Type::NONE))
	, factory{ factory }
	, spawnInterval{5.0f}
	, map{ map }
{
	++count;

	SetImage("X");
	boundsImg = "&";

	spawnTimer.SetTargetTime(spawnInterval);
}

SpawnPool::~SpawnPool()
{
	--count;
}

void SpawnPool::BeginPlay()
{
	super::BeginPlay();

	SpawnUnit();
	SpawnUnit();
	SpawnUnit();
}

void SpawnPool::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	spawnTimer.Tick(deltaTime);

	if (spawnTimer.IsTimeout() && !AUnit::IsOverMaxCount())
	{
		SpawnUnit();
		spawnTimer.Reset();
	}
}

void SpawnPool::Draw(Renderer& renderer)
{
	//super::Draw(renderer);

	for (int x = bounds.GetX(); x <= bounds.MaxX(); ++x)
	{
		for (int y = bounds.GetY(); y <= bounds.MaxY(); ++y)
		{
			renderer.WriteToBuffer({ x, y }, GetImage(), Color::White, 2);
		}
	}

	bounds.Draw(renderer, boundsImg, (Color)((int)color | (int)Color::Intensity), 2);
}

bool SpawnPool::SpawnUnit()
{
	int spawnBoundWidth = GetBounds().GetWidth() - 2;
	int spawnBoundHeight = GetBounds().GetHeight() - 2;

	Vector2I startPos = { GetBounds().GetX() + 1, GetBounds().GetY() + 1 };

	// 검사 최대 횟수
	int maxCount = spawnBoundWidth * spawnBoundHeight;
	for (int i = 0; i < maxCount; ++i)
	{
		Vector2I currentPos = startPos;
		currentPos.x += i % spawnBoundWidth;
		currentPos.y += i / spawnBoundWidth;

		// 스폰 검사
		if (map.CanMove(currentPos))
		{
			AUnit* unit = factory.CreatUnit(currentPos, GetOwnerTeam());
			GetOwner()->AddActor((Actor*)unit);
			map.SetOccupiedMap(currentPos, true);

			return true;
		}
	}

	// 스폰 실패
	return false;
}
