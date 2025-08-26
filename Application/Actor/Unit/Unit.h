#pragma once

#include "Actor/Actor.h"
#include "QuadTree/Bounds.h"

/// <summary>
/// 이 프로젝트에서 화면에 움직일 수 있는 것들을 정의한 클래스
/// - [] 쿼드 트리 자료구조에 넣을 수 있야 한다.
/// </summary>
class Unit : public Actor
{
	RTTI_DECLARATIONS(Unit, Actor)

public: // RAII
	Unit(const Vector2I& spawnPosition);
	virtual ~Unit();

public:
	//virtual void BeginPlay() override;
	//virtual void Tick(float deltaTime) override;
	//virtual void Render() override;

	//virtual void OnDestroy() override;

public: // GET SET

	Bounds GetBounds() const { return bounds; }

private: // FILD

	// 쿼드 트리에 넣기 위한 경계 상자
	Bounds bounds;
};