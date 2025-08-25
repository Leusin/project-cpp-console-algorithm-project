#include "Vector2F.h"

#include <iostream>
#include <cmath>
#include "Engine.h"

// 전역 변수 초기화.
Vector2F Vector2F::Zero = Vector2F(0.f, 0.f);
Vector2F Vector2F::One = Vector2F(1.f, 1.f);
Vector2F Vector2F::Up = Vector2F(0.f, 1.f);
Vector2F Vector2F::Right = Vector2F(1.f, 0.f);
Vector2F Vector2F::Down = Vector2F(0.f, -1.f);
Vector2F Vector2F::Left = Vector2F(-1.f, 0.f);

// 대각선 벡터는 길이가 1이 되도록 정규화된 값을 사용합니다.
const float invSqrt2 = 0.707106781f;
Vector2F Vector2F::UpRight = Vector2F(invSqrt2, invSqrt2);
Vector2F Vector2F::UpLeft = Vector2F(-invSqrt2, invSqrt2);
Vector2F Vector2F::DownRight = Vector2F(invSqrt2, -invSqrt2);
Vector2F Vector2F::DownLeft = Vector2F(-invSqrt2, -invSqrt2);

Vector2F::Vector2F(float x, float y)
	:x(x), y(y)
{
}

Vector2F::~Vector2F()
{
	if (value == nullptr)
	{
		return;
	}

	delete[] value;
}


Vector2F Vector2F::operator+(const Vector2F& other) const
{
	return Vector2F(x + other.x, y + other.y);
}

Vector2F Vector2F::operator-(const Vector2F& other) const
{
	return Vector2F(x - other.x, y - other.y);
}

Vector2F Vector2F::operator*(const float& scalar) const
{
	return Vector2F(x * scalar, y * scalar);
}

/*
bool Vector2::operator==(const Vector2& other) const
{
	const float epsilon = 0.01f; // 허용 가능한 오차
	return (std::abs(x - other.x) < epsilon) && (std::abs(y - other.y) < epsilon);
}
*/

Vector2F Vector2F::Normalize() const
{
	float magnitude = Magnitude();

	if (magnitude > 0) // 0보다 클 때만 나누기 연산을 수행
	{
		return { x / magnitude, y / magnitude };
	}

	__debugbreak();

	return Vector2F::Zero;
}

float Vector2F::Magnitude() const
{
	// 각 원소를 제곱 값을 모두 더한 뒤 제곱근 해줌

	return sqrt(SqrMagnitude());
}


float Vector2F::SqrMagnitude() const
{
	return (x * x + y * y);
}

const char* Vector2F::ToString()
{
	if (value == nullptr)
	{
		delete[] value;
	}

	value = new char[1024];
	sprintf_s(value, 1024, "(%d, %d)", static_cast<int>(x), static_cast<int>(y));

	return value;
}