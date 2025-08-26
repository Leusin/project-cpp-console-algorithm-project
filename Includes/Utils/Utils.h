#pragma once

#include <Windows.h>
#include <cstdlib>
#include "Math/Color.h"
#include "Math/Vector2I.h"
#include "Engine.h"

/// <summary>
/// 프로젝트에서 다양하게 사용할 유틸리티 함수 모음 
/// </summary>
namespace Utils
{
	/// <summary>
	/// 두 값을 서로 바꿀 때 사용하는 함수
	/// </summary>
	template<typename T>
	void Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	/// </summary>
	/// 콘솔 출력 핸들 가져오는 함수.
	/// </summary>
	inline HANDLE GetConsoleHandle() // 함수 재정의 문제를 피하기 위해 inline
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}

	/// <summary>
	/// 콘솔 커서 위치 이동
	/// </summary>
	inline void SetConsolePosition(COORD coord)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleCursorPosition(handle, coord);
	}

	/// <summary>
	/// 콘솔 커서 위치 이동
	/// </summary>
	inline void SetConsolePosition(const Vector2I& position)
	{
		SetConsolePosition(static_cast<COORD>(position));
	}

	/// <summary>
	/// 콘솔 텍스트 색성 설정
	/// </summary>
	/// <param name="color">WORD</param>
	inline void SetConsoleTextColor(WORD color)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleTextAttribute(handle, color);
	}

	/// <summary>
	/// 콘솔 텍스트 색성 설정
	/// </summary>
	/// <param name="color">Color</param>
	inline void SetConsoleTextColor(Color color)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleTextAttribute(handle, static_cast<WORD>(color));
	}

	/// <summary>
	/// 랜덤 정수 생성 함수
	/// </summary>
	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}


	inline float RandomFloat(float min, float max)
	{
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		return min +(max - min) * random;
	}

	//  직교 좌표계 → 화면 좌표계 변환 함수
	inline Vector2I OrthogonalToScreenCoords(const Vector2F& worldPos, const Vector2I& cameraPos)
	{
		Vector2I screenPos;

		// 1. 카메라 위치를 기준으로 상대적 위치를 계산합니다.
		screenPos.x = (int)round(worldPos.x) - cameraPos.x;
		screenPos.y = (int)round(worldPos.y) - cameraPos.y;

		// 2. Y축을 뒤집습니다. (y-up -> y-down)
		screenPos.y *= -1;

		// 3. 화면 중앙을 기준으로 옮깁니다.
		screenPos.x += Engine::halfWidth();
		screenPos.y += Engine::halfHeight();

		return screenPos;
	}

	// 화면 좌표계 → 직교 좌표계 변환 함수 
	inline Vector2F ScreenToOrthogonalCoords(const Vector2I& screenPos, const Vector2I& cameraPos)
	{
		Vector2F worldPos;

		// 1. 화면 중앙을 기준으로 상대적 위치를 계산합니다.
		worldPos.x = (float)screenPos.x - Engine::halfWidth();
		worldPos.y = (float)screenPos.y - Engine::halfHeight();

		// 2. Y축을 뒤집습니다. (y-down -> y-up)
		worldPos.y *= -1;

		// 3. 카메라 위치를 더해 최종 월드 좌표를 얻습니다.
		worldPos.x += cameraPos.x;
		worldPos.y += cameraPos.y;

		return worldPos;
	};
}

