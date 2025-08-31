#include "Territory.h"

#include <unordered_map>
#include "QuadTree/QuadTree.h"
#include "Render/Renderer.h"
#include "Actor/AUnit/AUnit.h"

Territory::Territory(int id, Vector2I position, Vector2I size, QuadTree& qTree, Team::Type initialOwner)
	: QEntity(position, Team::GetTeamColor(initialOwner), "X", size)
	, id{ id }
	, isContested{ false }
	, qTree{ qTree }
	, owner{ initialOwner }
	, boundsImg{ "x" }
{

}

void Territory::BeginPlay()
{
	super::BeginPlay();

	float targetTime = (float)(bounds.GetWidth() + bounds.GetHeight());
	captureTimer.SetTargetTime(targetTime);
}

void Territory::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 1. QuadTree를 이용해 현재 영토 내의 모든 유닛을 찾기
	CheckUnits();
	UpdateCaptureLogic(deltaTime);
}

void Territory::Draw(Renderer& renderer)
{
	// 일부러 부모 클래스 함수 호출 안 함

	for (int x = bounds.GetX(); x <= bounds.MaxX(); ++x)
	{
		for (int y = bounds.GetY(); y <= bounds.MaxY(); ++y)
		{
			//renderer.WriteToBuffer({ x, y }, GetImage(), color, 2);
			renderer.WriteToBuffer({ x, y }, GetImage(), (Color)((int)color | (int)Color::Intensity), 2);
		}
	}

	bounds.Draw(renderer, boundsImg, color, 2);
	//bounds.Draw(renderer, boundsImg, (Color)((int)color | (int)Color::Intensity), 2);
}

void Territory::SetOwnerTeam(const Team::Type& type)
{
	color = Team::GetTeamColor(type);
	owner = type;
	captureTimer.Reset();
}

void Territory::CheckUnits()
{
	nearbyUnits.clear();

	if (std::vector<QEntity*> intersects; qTree.Query(GetBounds(), intersects))
	{
		for (QEntity* entity : intersects)
		{
			if (entity->As<AUnit>())
			{
				AUnit* unit = (AUnit*)entity;
				nearbyUnits.push_back(unit);
			}
		}
	}
}

void Territory::UpdateCaptureLogic(float deltaTime)
{
	// 유닛이 하나도 없다면 아웃
	if (nearbyUnits.empty())
	{
		return;
	}

	// 유닛 수를 세어 다수결 팀을 결정
	std::unordered_map<Team::Type, int> unitCounts;
	for (AUnit* unit : nearbyUnits)
	{
		++unitCounts[unit->GetTeamType()];
	}

	// 가장 많은 유닛을 가진 팀을 찾음
	Team::Type majorityTeam = Team::Type::NONE;
	int maxCount = 0;
	isContested = false;

	for (const auto& pair : unitCounts)
	{
		if (pair.second > maxCount)
		{
			maxCount = pair.second;
			majorityTeam = pair.first;
			isContested = false;
		}
		// 2팀 이상 같은 수임: 분쟁 중
		else if (pair.second == maxCount)
		{
			isContested = true;
		}
	}

	// 분쟁중이 아닐 때만 타이머가 움직임
	if (!isContested)
	{
		// 다른팀이 점령 중인 경우
		if (owner != majorityTeam)
		{
			captureTimer.Tick(maxCount * deltaTime);
		}
		// 점령하지 않는 경우
		else
		{
			captureTimer.Tick(-deltaTime);
		}
	}

	// 타임 아웃이면 주인 바뀜
	if (captureTimer.IsTimeout())
	{
		SetOwnerTeam(majorityTeam);
	}

}