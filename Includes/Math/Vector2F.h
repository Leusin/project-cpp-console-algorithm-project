#pragma once

#include <windows.h>
#include "Core.h"

class Engine_API Vector2F
{
public:
	Vector2F(float x = 0.f, float y = 0.f);
	~Vector2F();

	Vector2F operator+(const Vector2F& other) const;
	Vector2F operator-(const Vector2F& other) const;
	Vector2F operator*(const float& scalar) const;

	/// <summary>
	/// 벡터 정규화 취소.
	/// </summary>
	Vector2F Normalize() const;

	/// <summary>
	/// 벡터 크기 구하기
	/// </summary>
	float Magnitude() const;

	/// <summary>
	///만약 벡터의 크기 비교만 필요하고, 실제 크기 값을 얻을 필요 없을 때 사용
	/// </summary>
	float SqrMagnitude() const;

	// 성분들을 절댓값으로 만드는 함수
	void Abs();
	Vector2F GetAbs() const;

	const char* ToString();

	// 직교 좌표계에서의 방향 벡터
	static Vector2F Zero;
	static Vector2F One;
	static Vector2F Up;
	static Vector2F Down;
	static Vector2F Right;
	static Vector2F Left;
	static Vector2F UpRight;
	static Vector2F UpLeft;
	static Vector2F DownRight;
	static Vector2F DownLeft;

public:
	float x = 0.f;
	float y = 0.f;

private:
	char* value = nullptr;
};
