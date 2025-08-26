#pragma once


#include "Math/Vector2I.h"
#include "Math/Color.h"

class Engine_API Renderer
{
	friend class Engine;

public: // RTTI

	Renderer();
	~Renderer();

public: // MESSAGE

	// 초기화/정리 함수
	// 이미지 버퍼에 문자/색상/정렬 순서 기록
	void WriteToBuffer(const Vector2I& position, const char* image, Color color = Color::White, int sortingOrder = 0);

private: // METHOD

	void Initialize(const struct EngineSettings* settings);

	void CleanUp();

	void Render();

	void PresentImmediately(); // 언제 사용할지 잘 모르겠다

	class ScreenBuffer* GetRenderer() const;

	// 현재 백버퍼를 프론트 버퍼로 교환
	void Present();

	// 화면을 지움
	void Clear();

	// 글자 버퍼 지우는 함수.
	void ClearImageBuffer();

private: //FIELD

	struct ImageBuffer* imageBuffer = nullptr;
	class ScreenBuffer* renderTargets[2] = { };
	int currentRenderTargetIndex = 0;

	const EngineSettings* engineSettings;
};

