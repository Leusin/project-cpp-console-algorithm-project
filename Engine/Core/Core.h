#pragma once

// dll 외부로 템플릿을 내보낼 때 발생하는 경고 제거
// warning C4251: ...에서는 ... 의 클라이언트에서 DLL 인터페이스를 사용하도록 지정해야 함
#pragma warning(disable: 4251)
// RTTI 클래스가 인지하고 사용 증
// warning C4172: 지역 변수 또는 임시 의 주소를 반환하는 중
#pragma warning(disable: 4172)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#if BuildEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif 

/// <summary>
/// 메모리 정리 함수
/// </summary>
template<typename T>
void SafeDelete(T*& target)
{
	if (target)
	{
		delete target;
		target = nullptr;
	}
}

/// <summary>
/// 배열 메모리 정리 함수
/// </summary>
template<typename T>
void SafeDeleteArray(T*& target)
{
	if (target)
	{
		delete[] target;
		target = nullptr;
	}
}