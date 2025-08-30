#pragma once

#include <vector>
#include "Actor/QEntity/QEntity.h"
#include "Actor/AUnit/Team.h"
#include "Utils/Timer.h"

/// <summary>
/// 땅따먹기 가능 영역
/// - 영역 크기에 따라 점령할 수 있는 시간이 다름
/// - 현재 영역 안에 있는 유닛 종류, 수 별로 점령할 수 있는 시간이 다름
/// - 점령된 팀의 유닛이 이 위에 있음 체력 회복됨
/// </summary>
class Territory : public QEntity
{
	RTTI_DECLARATIONS(Territory, QEntity)

public: // RAII

	Territory(int id, Vector2I position, Vector2I size, class QuadTree& qTree, Team::Type initialOwner = Team::Type::NONE);
	virtual ~Territory() = default;
public: // EVENT

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void UpdateCaptureLogic(float deltaTime);
	virtual void Draw(class Renderer& renderer) override;

public: // GET SET

	const int& GetId() const { return id; }

	const Team::Type& GetOwnerTeam() const { return owner; }
	void SetOwnerTeam(const Team::Type& type);

private: // METHOD

	void CheckUnits();

protected: // FILD

	const char* boundsImg;

private: // FILD
	const int id;

	bool isContested;

	Team::Type owner;

	Timer captureTimer;

	class QuadTree& qTree;

	std::vector<class AUnit*> nearbyUnits;

};

