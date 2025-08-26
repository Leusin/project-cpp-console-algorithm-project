#include "ScreenBuffer.h"

#include <iostream>
#include "Engine.h"

ScreenBuffer::ScreenBuffer(const Vector2I& screenSize)
	: screenSize(screenSize)
{
	unsigned long accessMode = GENERIC_READ | GENERIC_WRITE; // 접근 모드 설정(읽기, 쓰기)
	unsigned long shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE; // 공유 모드 설정 (파일 입출력).

	// 콘솔 버퍼 생성.
	buffer = CreateConsoleScreenBuffer(accessMode, shareMode, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// 예외처리: 버퍼 생성 실패.
	if (buffer == INVALID_HANDLE_VALUE)
	{
		__debugbreak();
		std::cout << "버퍼 생성 실패\n";
		return;
	}

	// 콘솔 창 크기 설정.
	SMALL_RECT rect{ 0, 0, (short)screenSize.x, (short)screenSize.y };
	BOOL setWindowsInfoResult = SetConsoleWindowInfo(buffer, true, &rect);
	if (!setWindowsInfoResult)
	{
		std::cerr << "Failed to set console window size\n";
		__debugbreak();
	}

	// 콘솔 버퍼 크기 설정.
	BOOL setScreenBufferSizeResult = SetConsoleScreenBufferSize(buffer, Vector2I(screenSize.x + 1, screenSize.y + 1));
	if (!setScreenBufferSizeResult)
	{
		std::cerr << "Failed to set screen buffer size\n";
		__debugbreak();
	}

	// 커서 안보이게 설정.
	CONSOLE_CURSOR_INFO info{ 1, FALSE };
	SetConsoleCursorInfo(buffer, &info);
}

ScreenBuffer::ScreenBuffer(HANDLE console, const Vector2I& screenSize)
	: screenSize(screenSize), buffer(console)
{
	// 콘솔 창 크기 설정.
	SMALL_RECT rect{ 0, 0, (short)screenSize.x, (short)screenSize.y };
	BOOL setWindowsInfoResult = SetConsoleWindowInfo(buffer, true, &rect);
	if (!setWindowsInfoResult)
	{
		std::cerr << "Failed to set console window size\n";
		__debugbreak();
	}

	// 콘솔 버퍼 크기 설정.
	BOOL setScreenBufferSizeResult = SetConsoleScreenBufferSize(buffer, Vector2I(screenSize.x + 1, screenSize.y + 1));
	if (!setScreenBufferSizeResult)
	{
		std::cerr << "Failed to set screen buffer size\n";
		__debugbreak();
	}

	// 커서 안보이게 설정.
	CONSOLE_CURSOR_INFO cursorInfo{ 1, FALSE };
	SetConsoleCursorInfo(buffer, &cursorInfo);
}

ScreenBuffer::~ScreenBuffer()
{
	// 생성한 핸들 해제.
	if (buffer)
	{
		CloseHandle(buffer);
	}
}

void ScreenBuffer::Clear()
{
	// 커서 위치.
	Vector2I position;

	// 콘솔에 기록한 글자 수를 반환받기 위한 변수.
	DWORD writtenCount = 0;

	// 화면 버퍼 전체에 빈 문자열 기록.
	// 콘솔 전체에 한 글자를 입력할 때 사용 (memset과 비슷).
	FillConsoleOutputCharacter(buffer, ' ', ((int)screenSize.x + 1) * (int)screenSize.y + 1, position, &writtenCount);

}

void ScreenBuffer::Render(CHAR_INFO* charInfo)
{
	// 버퍼 위치 초기화.
	Vector2I bufferPosition;

	// 기록할 영역의 크기.
	SMALL_RECT writeRegion{ 0, 0, (short)screenSize.x - 1, (short)screenSize.y - 1 };

	// 전달된 문자 정보를 버퍼에 기록.
	WriteConsoleOutputA(buffer, charInfo, screenSize, bufferPosition, &writeRegion);
}
