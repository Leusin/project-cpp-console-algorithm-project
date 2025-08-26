#pragma once

#include <Windows.h>
#include "Core.h"

struct ImageBuffer
{
	ImageBuffer(int bufferCount)
	{
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(charInfoArray, 0, sizeof(int) * bufferCount);
	}

	~ImageBuffer()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	// 콘솔에 그릴 때 사용할 구조체 (문자,색상 값 저장).
	CHAR_INFO* charInfoArray = nullptr;

	// 해당 위치에 그릴지를 판단할 때 사용할 뎁스 값(sortingOrder).
	int* sortingOrderArray = nullptr;
};
