#include "Actor.h"

#include <iostream>
#include "Engine.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include "Render/Renderer.h"

Actor::Actor(const char* image, Color color, const Vector2I& position)
	: color(color), position(position)
{
	// iamge 초기화
	width = (int)strlen(image); // 문자열 길이
	this->image = new char[width + 1]; // 문자열 메모리 할당
	strcpy_s(this->image, width + 1, image); // 문자열 복사
}

Actor::~Actor()
{
	SafeDeleteArray(image); // 문자열 메모리 해제
}

void Actor::BeginPlay()
{
	if (hasBegonPlay)
	{
		return;
	}

	hasBegonPlay = true;
}

void Actor::Tick(float deltaTime)
{
	// autoDestroy가 활성화된 경우에만 lifetime을 감소시킵니다.
	if (autoDestroy)
	{
		lifetime -= deltaTime;

		// lifetime이 0 이하로 떨어졌고, 아직 파괴되지 않았다면 파괴합니다.
		if (lifetime <= 0.f && isExpired == false)
		{
			Destroy();
		}
	}
}

void Actor::Draw(Renderer& renderder)
{
	renderder.WriteToBuffer(position, image, color, sortingOrder);
}

void Actor::OnDestroy()
{
}

bool Actor::TestIntersect(const Actor* const other)
{
	// AABB(Axis Aligned Bounding Box)
	// Note: 현재 엑터 구조상 세로는 크기가 없음.
	//		따라서 가로 위치만 고려

	int xMin = (int)Position().x;
	int xMax = (int)Position().x + (int)width - 1;

	// 충돌 비교할 다른 액터의 x 좌표 정보
	int otherXMin = (int)other->Position().x;
	int otherXMax = (int)other->Position().x + (int)(other->width) - 1;

	// 안 겹치는 조건
	if (otherXMin > xMax) // 다른 액터의 왼쪽 경계가 내 오른쪽 경계보다 더 오른쪽
	{
		return false;
	}

	if (otherXMax < xMin) // 다른 액터의 오른쪽 경계가 내 왼쪽 경계보다 더 왼쪽
	{
		return false;
	}

	// TODO: 기본값 변경

	return Position().y == other->Position().y;
}

void Actor::Destroy()
{
	if(isExpired == true)
	{
		return;
	}

	isExpired = true; // 삭제 요청 설정
	owner->DestroyActor(this); // 레벨에게 삭제 요청
	OnDestroy();
}

Vector2I Actor::Position() const
{
	return position;
}

void Actor::SetPosition(const Vector2I& newPosition)
{
	if (position == newPosition)
	{
		return;
	}

	/*
	// 오른쪽 가장자리 혹은 맨 위로 벗어났는가
	if (newPosition.x < 0 || newPosition.y < 0)
	{
		return;
	}
	// 왼쪽 가장자리로 벗어났는가
	if (newPosition.x + width - 1 > Engine::Get().Width())
	{
		return;
	}
	// 아래 범위를 벗어났는가
	if (newPosition.y + 1 > Engine::Get().Height())
	{
		return;
	}
	*/

	position = newPosition;
}

unsigned int Actor::GetSortingOrder() const
{
	return sortingOrder;
}

void Actor::SetSortingOrder(unsigned int sortingOrder)
{
	this->sortingOrder = sortingOrder;
}

Level* Actor::GetOwner() const
{
	return owner;
}

void Actor::SetOwner(Level* owner)
{
	this->owner = owner;
}

void Actor::SetImage(const char* newImage)
{
	// 새 이미지 문자열이 기존 이미지와 동일한지 확인
	if (this->image != nullptr && strcmp(this->image, newImage) == 0)
	{
		return;
	}

	int newWidth = (int)strlen(newImage);

	//새 이미지의 길이가 기존 이미지와 같은지 확인
	if (image != nullptr && newWidth == this->width)
	{
		// 길이가 같으므로 메모리 재할당 없이 문자열만 덮어쓰기
		strcpy_s(this->image, newWidth + 1, newImage);
	}

	SafeDelete(this->image);

	// iamge 초기화
	this->image = new char[newWidth + 1]; // 문자열 메모리 할당
	strcpy_s(this->image, newWidth + 1, newImage); // 문자열 복사
}

void Actor::SetLifetime(float newLifetime)
{
	// 입력값 확인.
	if (newLifetime <= 0.0f)
	{
		return;
	}

	// 수명 주기 설정.
	lifetime = newLifetime;

	// 자동 제거 옵션 활성화.
	autoDestroy = true;
}

