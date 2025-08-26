#include "Input.h"


Input* Input::instance = nullptr;

Input::Input()
{
	instance = this; // 싱글턴 인스턴스 변수 설정

	hInput = GetStdHandle(STD_INPUT_HANDLE);

	// 콘솔 입력 모드 설정
	DWORD dwMode = 0;
	GetConsoleMode(hInput, &dwMode);

	// 마우스 입력 활성화
	dwMode |= ENABLE_MOUSE_INPUT;

	// 퀵 에디트 모드 비활성화 (터미널 환경에선 중요)
	dwMode &= ~ENABLE_QUICK_EDIT_MODE; 

	SetConsoleMode(hInput, dwMode);
}

Input::~Input()
{
	// 프로그램 종료 시 원래 모드로 되돌리기
	DWORD dwMode = 0;
	GetConsoleMode(hInput, &dwMode);
	dwMode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hInput, dwMode);
}

bool Input::GetKey(int KeyCode)
{
	return keyStates[KeyCode].isKeyDown;
}

bool Input::GetKeyDown(int KeyCode)
{
	return !keyStates[KeyCode].previouseKeyDown && keyStates[KeyCode].isKeyDown;
}

bool Input::GetKeyUp(int KeyCode)
{
	return keyStates[KeyCode].previouseKeyDown && !keyStates[KeyCode].isKeyDown;
}

bool Input::GetMouseClick()
{
	return isMouseClick;
}

int Input::GetMouseX() const
{
	return mouseX;
}

int Input::GetMouseY() const
{
	return mouseY;
}

Input& Input::Get()
{
	return *instance;
}

void Input::ProcessInput()
{
	// 모든 키 상태를 다음 프레임을 위해 저장
	SavePreviousKeyStates();

	// 키입력 확인
	/*for (int keyCode = 0; keyCode < 256; ++keyCode)
	{
		keyStates[keyCode].isKeyDown = GetAsyncKeyState(keyCode) & 0x80000;
	}*/

	// 마우스 상태 초기화
	isMouseClick = false;

	INPUT_RECORD irInBuf[128];
	DWORD cNumRead;

	// 입력 버퍼에 있는 모든 이벤트를 읽어옴
	if (PeekConsoleInput(hInput, irInBuf, 128, &cNumRead) && cNumRead > 0) 
	{
		if(!ReadConsoleInput(hInput, irInBuf, cNumRead, &cNumRead))
		{
			return;
		}

		for (DWORD i = 0; i < cNumRead; i++) 
		{
			switch (irInBuf[i].EventType) 
			{
			case KEY_EVENT:
				// 키보드 이벤트 처리
				keyStates[irInBuf[i].Event.KeyEvent.wVirtualKeyCode].isKeyDown = irInBuf[i].Event.KeyEvent.bKeyDown;
				break;
			case MOUSE_EVENT:
				// 마우스 이벤트 처리
				HandleMouseEvent(irInBuf[i].Event.MouseEvent);
				break;
			}
		}
	}
}

void Input::SavePreviousKeyStates()
{
	for (int keyCode = 0; keyCode < 255; ++keyCode)
	{
		keyStates[keyCode].previouseKeyDown = keyStates[keyCode].isKeyDown;
	}
}

void Input::HandleMouseEvent(const MOUSE_EVENT_RECORD& mouseEvent)
{
	// 마우스 좌표 업데이트
	mouseX = mouseEvent.dwMousePosition.X;
	mouseY = mouseEvent.dwMousePosition.Y;

	// 마우스 클릭 상태 업데이트
	if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) 
	{
		isMouseClick = true;
	}
	else 
	{
		isMouseClick = false;
	}
}
