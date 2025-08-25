#pragma once

#include<Windows.h>
#include "Core.h"
#include "Input.h"
#include "Math/Vector2I.h"
#include "Math/Color.h"

/**
 * TODO: 언젠가 Engine의 싱글턴 지우고 싶다.
 */

 /// <summary>
 /// 엔진 설정 구조체
 /// </summary>
struct EngineSettings
{
	int width = 0; // 콘솔 화면 가로 너비
	int height = 0; // 콘솔 화면 세로 너비
	float framerate = 0.0f; // 타겟 프레임 속도
};

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


class Engine_API Engine
{
public:
	Engine();
	virtual ~Engine();

	void AddLevel(class Level* newLevel);

	void Run(); // 엔진 루프

	/// <summary>
	/// 문자열 그리기 요청 함수.
	/// </summary>
	/// <param name="position">기록할 문자 위치</param>
	/// <param name="image">기록할 문자</param>
	/// <param name="color">문자의 색상</param>
	/// <param name="sortingOrder">이미지 순서</param>
	void WriteToBuffer(const Vector2I& position, const char* image, Color color = Color::White, int sortingOrder = 0);
	// 버퍼를 곧바로 교환 요청할 때 사용하는 함수.
	void PresentImmediately();

	virtual void CleanUp(); // 메모리 해제

	void Quit(); // 종료

	static Engine& Get();

	// 화면의 너비
	int Width() const;
	int halfWidth() const;
	// 화면의 높이
	int Height() const;
	int halfHeight() const;
	// 화면 중앙
	Vector2I ScreenCenter();

	class ScreenBuffer* GetRenderer() const;

	/// <summary>
	///  직교 좌표계 → 화면 좌표계 변환 함수
	/// </summary>
	Vector2I OrthogonalToScreenCoords(const Vector2F& worldPos, const Vector2I& cameraPos = Vector2I::Zero);

	/// <summary>
	/// 화면 좌표계 → 직교 좌표계 변환 함수 
	/// </summary>
	Vector2F ScreenToOrthogonalCoords(const Vector2I& screenPos, const Vector2I& cameraPos = Vector2I::Zero);

protected:
	class Level* mainLevel = nullptr; // 메인 레벨

private:
	//
	//메인 루프 함수
	//
	
	void BeginPlaye();
	void Tick(float deltaTime = 0.0f);

	// 액터 그리기 (백버퍼에 기록)
	void Render();

	//
	// 초기화 함수
	//
	
	// 엔진 설정
	void LoadEngineSettings(); 

	//
	// Render 함수
	//

	// 화면 지우는 함수 (전체를 빈 문자열로 설정).
	void Clear();

	// 버퍼 교환 함수 (프론트 버퍼 <-> 백버퍼)
	void Present();

	// 글자 버퍼 지우는 함수.
	void ClearImageBuffer();

private:
	void DisableToResizeWindow();

	bool isQuit = false; // 엔진 종료 플래그

	Input input; // 입력 관리자

	EngineSettings settings; // 엔진 설정

	//
	// Render 관련
	//

	//CHAR_INFO* imageBuffer = nullptr;
	// 문자, 색상, 뎁스(sortingOrder)까지 저장하는 버퍼.
	ImageBuffer* imageBuffer = nullptr;

	class ScreenBuffer* renderTargets[2] = { }; // 이중 버퍼.
	
	int currentRenderTargetIndex = 0; // 백버퍼로 사용하는 렌더 타겟의 인덱스.

	//
	// 싱글턴 변수
	//

	static Engine* instance;
};
