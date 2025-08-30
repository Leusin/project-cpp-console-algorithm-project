#pragma once

#include "Actor/QEntity/QEntity.h"
#include "Actor/AUnit/Team.h"

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

	Territory(int id, Vector2I position, Vector2I size, const class QuadTree& qTree, Team::Type initialOwner = Team::Type::NONE);
	
public: // GET SET

	void SetOwner(const Team::Type& type);
	const int& GetId() const { return id; }

public: // EVENT

	virtual void Draw(class Renderer& renderer) override;

private: // FILD
	const int id;

	Team::Type owner;

	const class QuadTree& qTree;
};

