#include "Engine.h"

#include <iostream>
#include "Level/level.h"
#include "Utils/Utils.h"


// static 멤버 및 함수 변수 정의

EngineSettings Engine::settings;

// 전역 변수 콘솔 창 이벤트 처리에 사용
Engine* instance = nullptr;

// 콘솔창 이벤트 처리 함수
BOOL WINAPI ConsoleMessageProcedure(DWORD ctrlType)
{
	switch (ctrlType)
	{
	case CTRL_CLOSE_EVENT:
	{
		instance->Quit();
		return true;
	}
	}
	return true;
}

Engine::Engine()
{
	// 콘솔 창 크기 변경 안되도록 설정
	// "관리자 모드에서만 제대로 실행됨"
	DisableToResizeWindow();

	// 엔진 설정 로드
	LoadEngineSettings();

	// 렌더러 초기화
	renderer.Initialize(&settings);

	// 랜덤 종자값(Seed) 생성
	srand(static_cast<unsigned int>(time(nullptr)));

	// 콘솔창 이벤트 등록
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	// 싱글턴 아님
	instance = this;
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::CleanUp()
{
	// 렌더러 정리.
	renderer.CleanUp();

	// 레벨 삭제.
	SafeDelete(mainLevel);
}

void Engine::Run()
{
	// 이전 프레임과 현재 프레임의 시간 측정
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	LARGE_INTEGER previousTime = currentTime;

	// 하드웨어의 정밀도(주파수) 가져오기
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 타겟 프레임 설정
	float targetFrameRate = (settings.framerate != 0.0f) ? settings.framerate : 60.f;

	// 한 프레임에 걸리는 시간
	float oneFrameTime = 1.f / targetFrameRate;

	float slowUpdateAccumulator = 0.0f;
	const float slowUpdateInterval = oneFrameTime * 3;

	// 메인 루프
	while (true)
	{
		// 종료 요청이 들어왔는지 확인
		if (isQuit)
		{
			break;
		}

		// 델타 타임: 현재 프레임과 이전 프레임의 시간 차이 계산
		QueryPerformanceCounter(&currentTime);
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

		// 입력 처리
		input.ProcessInput();

		// 고정 프레임
		if (deltaTime >= oneFrameTime)
		{
			BeginPlaye();
			Tick(deltaTime);
			Render();

			// 프레임이 높은 업데이트를 위한 구간
			slowUpdateAccumulator += deltaTime;
			if (slowUpdateAccumulator >= slowUpdateInterval)
			{
				SlowTick(slowUpdateAccumulator);
			}

			// 이전 프레임 시간 갱신
			previousTime = currentTime;

			// 키 상태 저장
			input.SavePreviousKeyStates();


			// 이전 프레임에 추가 및 삭제 요청 처리
			if (mainLevel)
			{
				mainLevel->ProcessAddAndDestroyActors();
			}
		}
	}
}

void Engine::AddLevel(Level* newLevel)
{
	SafeDelete(mainLevel);
	mainLevel = newLevel;
}

void Engine::Quit()
{
	isQuit = true;
}

int Engine::Width()
{
	return settings.width;
}

int Engine::halfWidth()
{
	return settings.width / 2;
}

int Engine::Height()
{
	return settings.height;
}

int Engine::halfHeight()
{
	return settings.height / 2;
}

void Engine::BeginPlaye()
{
	// LEVEL
	if (mainLevel)
	{
		mainLevel->BeginPlay();
	}
}

void Engine::Tick(float deltaTime)
{
	// LEVEL
	if (mainLevel)
	{
		mainLevel->Tick(deltaTime);
	}
}

void Engine::SlowTick(float deltaTime)
{
	// LEVEL
	if (mainLevel)
	{
		mainLevel->SlowTick(deltaTime);
	}
}

void Engine::Render()
{
	// LEVEL
	if (mainLevel)
	{
		mainLevel->Draw(renderer);
	}

	renderer.Render();
}

void Engine::LoadEngineSettings()
{
	// 파일 열기
	FILE* file = nullptr;

	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");

	if (file == nullptr)
	{
		std::cout << "Failed to load engine settigs. \n";
		__debugbreak();
		return;
	}

	/// 로드 
	fseek(file, 0, SEEK_END); // FP(file position) 포인터를 끝위치로 옮기기
	size_t fileSize = ftell(file); // 위치 구하기
	rewind(file); // FP 포인터를 처음으로 

	char* buffer = new char[fileSize + 1];

	size_t readSize = fread(buffer, sizeof(char), fileSize, file); // 내용 읽기

	/// 파싱(구문 해석)
	char* context = nullptr;
	char* token = nullptr;

	token = strtok_s(buffer, "\n", &context);

	// 구문 분석
	while (token != nullptr)
	{
		// 키 = 값 분리
		char header[10] = {};

		// 아래 구문이 제대로 동작하려면 키와 값 사이의 빈칸이 있어야 함
		sscanf_s(token, "%s", header, 10);

		// 헤더 문자열 비교
		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}
		else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}
		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}

		// 그 다음줄 분리
		token = strtok_s(nullptr, "\n", &context);
	}

	SafeDeleteArray(buffer); // 버퍼 해제

	// 파일 닫기
	fclose(file);
}

void Engine::DisableToResizeWindow()
{
	// 콘솔 창 핸들 가져오기.
	HWND window = GetConsoleWindow();

	// 콘솔 창에 설정된 스타일 값 가져오기.
	LONG style = GetWindowLong(window, GWL_STYLE);

	// 콘솔 창 스타일에서 크기 조절 관련 옵션 제거.
	style &= ~WS_MAXIMIZEBOX;
	style &= ~WS_SIZEBOX;

	// 콘솔창에 변경된 스타일 적용.
	SetWindowLongW(window, GWL_STYLE, style);
}
