#pragma once

#pragma once

#include "Actor/Actor.h"
#include "QuadTree/Bounds.h"

/// <summary>
/// 이 프로젝트에서 화면에 움직일 수 있는 것들을 정의한 클래스
/// </summary>
class QEntity : public Actor
{
	RTTI_DECLARATIONS(QEntity, Actor)

public: // RAII

	QEntity(const Vector2I& position, Color color = Color::White, const char* image = "U", const Vector2I& size = Vector2I::Zero)
		: Actor(position, color, image)
		, bounds{ position , size.x, size.x }
	{
	}

	virtual ~QEntity() = default;

public: // GET SET

	Bounds GetBounds() const { return bounds; }
	void SetBounds(const Bounds& bounds) { this->bounds = bounds; }

protected: // FILD

	// 쿼드 트리에 넣기 위한 경계 상자
	Bounds bounds;
};