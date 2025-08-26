#pragma once

#include <Windows.h>
#include "Math/Vector2I.h"

/// <summary>
/// 
/// </summary>
class ScreenBuffer
{
	friend class Renderer;

public:
	// 새 버퍼 생성 시 사용
	ScreenBuffer(const Vector2I& screenSize);

	// 콘솔에 전달된 핸들을 통해 콘솔 버퍼 생성
	ScreenBuffer(HANDLE console, const Vector2I& screenSize);

	~ScreenBuffer();

	void Clear(); // 버퍼 지우기
 
	void Render(CHAR_INFO* charInfo); // 콘솔 버퍼에 문자열 버퍼 사용시

private:
	HANDLE buffer = nullptr; // 콘솔 핸들
	
	Vector2I screenSize; // 화면 크기
};