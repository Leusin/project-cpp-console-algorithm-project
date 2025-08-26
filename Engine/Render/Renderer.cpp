#include "Renderer.h"

#include "Engine.h"
#include "ImageBuffer.h"
#include "Utils/Utils.h"
#include "ScreenBuffer.h"

Renderer::Renderer()
	: engineSettings{ nullptr }
{
	// 콘솔 커서 끄기
	_CONSOLE_CURSOR_INFO consoleCursorInfo;
	consoleCursorInfo.bVisible = false;
	consoleCursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInfo); // 커서가 보이지 않도록 
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(const EngineSettings* settings)
{
	if (!settings)
	{
		__debugbreak();
	}

	engineSettings = settings;

	Vector2I screenSize(settings->width, settings->height);
	imageBuffer = new ImageBuffer((settings->width + 1) * settings->height + 1);

	renderTargets[0] = new ScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE), screenSize);
	renderTargets[1] = new ScreenBuffer(screenSize);

	Present(); // 버퍼 교환.
}

void Renderer::CleanUp()
{
	// 문자 버퍼 삭제.
	SafeDelete(imageBuffer);

	// 렌더 타겟 삭제.
	SafeDelete(renderTargets[0]);
	SafeDelete(renderTargets[1]);
}

void Renderer::Render()
{
	GetRenderer()->Render(imageBuffer->charInfoArray); // 백 버퍼에 그리기

	Present(); // 버퍼 교환

	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	Clear(); // 화면 지우기
}

void Renderer::PresentImmediately() // 언제 사용할지 잘 모르겠다
{
	GetRenderer()->Render(imageBuffer->charInfoArray); // 백 버퍼에 그리기
	Present(); // 버퍼 교환
}

ScreenBuffer* Renderer::GetRenderer() const
{
	return renderTargets[currentRenderTargetIndex];
}

/// <summary>
/// 문자열 그리기 요청 함수.
/// </summary>
/// <param name="position">기록할 문자 위치</param>
/// <param name="image">기록할 문자</param>
/// <param name="color">문자의 색상</param>
/// <param name="sortingOrder">이미지 순서</param>
void Renderer::WriteToBuffer(const Vector2I& position, const char* image, Color color, int sortingOrder)
{
	// 문자열 길이.
	int length = static_cast<int>(strlen(image));

	// 문자열 기록.
	for (int ix = 0; ix < length; ++ix)
	{
		// 글자의 현재 x, y 좌표를 계산
		int currentX = position.x + ix;
		int currentY = position.y;

		// 현재 글자의 위치가 화면 범위 안에 있는지 확인
		if (currentX < 0 || currentX >= engineSettings->width || currentY < 0 || currentY >= engineSettings->height)
		{
			continue;
		}

		// 화면 범위 안에 있다면, 버퍼 인덱스를 계산
		int index = (currentY * engineSettings->width) + currentX;

		if (imageBuffer->sortingOrderArray[index] > sortingOrder)
		{
			continue;
		}

		// 버퍼에 문자/색상 기록.
		imageBuffer->charInfoArray[index].Char.AsciiChar = image[ix];
		imageBuffer->charInfoArray[index].Attributes = (WORD)color;

		imageBuffer->sortingOrderArray[index] = sortingOrder;
	}
}

void Renderer::Present()
{
	// 버퍼 교환.
	SetConsoleActiveScreenBuffer(GetRenderer()->buffer);

	// 인덱스 뒤집기. 1->0, 0->1.
	currentRenderTargetIndex = 1 - currentRenderTargetIndex;
}

void Renderer::Clear()
{
	ClearImageBuffer();
	GetRenderer()->Clear();
}

void Renderer::ClearImageBuffer()
{
	// 글자 버퍼 덮어쓰기.
	for (int y = 0; y < engineSettings->height; ++y)
	{
		for (int x = 0; x < engineSettings->width; ++x)
		{
			int index = (y * (engineSettings->width)) + x;
			CHAR_INFO& buffer = imageBuffer->charInfoArray[index];
			buffer.Char.AsciiChar = ' ';
			buffer.Attributes = 0;
			imageBuffer->sortingOrderArray[index] = -1;
		}

		// 각 줄 끝에 개행 문자 추가.
		int index = (y * (engineSettings->width)) + engineSettings->width;
		CHAR_INFO& buffer = imageBuffer->charInfoArray[index];
		buffer.Char.AsciiChar = '\n';
		buffer.Attributes = 0;
		imageBuffer->sortingOrderArray[index] = -1;
	}

	// 마지막에 널 문자 추가.
	int index = (engineSettings->width) * engineSettings->height + 1;
	CHAR_INFO& buffer = imageBuffer->charInfoArray[index];
	buffer.Char.AsciiChar = '\0';
	buffer.Attributes = 0;
	imageBuffer->sortingOrderArray[index] = -1;
}