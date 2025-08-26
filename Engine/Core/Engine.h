#pragma once

#include<Windows.h>
#include "Core.h"
#include "Input.h"
#include "Math/Vector2I.h"
#include "Math/Color.h"
#include "Render/Renderer.h"

 /// <summary>
 /// 엔진 설정 구조체
 /// </summary>
struct EngineSettings
{
	int width = 0; // 콘솔 화면 가로 너비
	int height = 0; // 콘솔 화면 세로 너비
	float framerate = 0.0f; // 타겟 프레임 속도
};

class Engine_API Engine
{
public: // RTTI

	Engine();
	virtual ~Engine();

	virtual void CleanUp(); // 메모리 해제

public: // MESSAGE

	// 엔진 루프
	void Run(); 

	// 레벨 추가
	void AddLevel(class Level* newLevel);

	// 종료
	void Quit();

public: // GET SET

	// 화면의 너비
	static int Width();
	static int halfWidth();

	// 화면의 높이
	static int Height();
	static int halfHeight();

	// 화면 중앙
	Vector2I ScreenCenter() { return { halfWidth() , halfHeight() }; }

private: // METHOD
	
	//
	// Main Loop 함수
	//

	// 갓 생성한 액터 초기화
	void BeginPlaye();

	// 업데이트 (입력, 게임 로직)
	void Tick(float deltaTime = 0.0f);

	// 그리기 (백버퍼에 기록)
	void Render();

	//
	// 기타 초기화
	//
	
	// 엔진 설정
	static void LoadEngineSettings(); 

	// 리사이즈 제한 설정
	void DisableToResizeWindow();

protected: // FIELD

	class Level* mainLevel = nullptr; // 메인 레벨

private: // FIELD

	// 엔진 종료 플래그
	bool isQuit = false; 

	// 입력 관리자
	Input input; 

	// 렌더러
	Renderer renderer;

	// 엔진 설정
	static EngineSettings settings;

};
