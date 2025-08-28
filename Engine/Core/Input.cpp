#include "Input.h"

Input* Input::instance = nullptr;

Input::Input()
	:hStdin{ GetStdHandle(STD_INPUT_HANDLE) }
{
	if (hStdin == INVALID_HANDLE_VALUE) // GetStdHandle 실패 체크
	{
		__debugbreak();
	}

	// 콘솔 기존 입력 모드 저장
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) //GetConsoleMode 실패 체크
	{
		__debugbreak();
	}

	// 콘솔 입력 모드 설정
	// 마우스 입력(ENABLE_MOUSE_INPUT) 활성화
	// 마우스 입력을 방해하기 때문에 빠른 편집 모드(ENABLE_QUICK_EDIT_MODE) 비활성화
	DWORD fdwMode = (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS) & ~ENABLE_QUICK_EDIT_MODE;

	if (!SetConsoleMode(hStdin, fdwMode)) //SetConsoleMode 실패 체크
	{
		__debugbreak();
	}

	instance = this; // 싱글턴 인스턴스 변수 설정
}

Input::~Input()
{
	// 프로그램 종료 시 원래 모드로 되돌리기
	SetConsoleMode(hStdin, fdwSaveOldMode);
}

bool Input::GetKey(int KeyCode) const
{
	return keyStates[KeyCode].isKeyDown;
}

bool Input::GetKeyDown(int KeyCode) const
{
	return !keyStates[KeyCode].previouseKeyDown && keyStates[KeyCode].isKeyDown;
}

bool Input::GetKeyUp(int KeyCode) const
{
	return keyStates[KeyCode].previouseKeyDown && !keyStates[KeyCode].isKeyDown;
}

bool Input::GetMouse(MounseButton button) const
{
	return mouseStates[(int)button].isKeyDown;
}

bool Input::GetMouseDown(MounseButton button) const
{
	return !mouseStates[(int)button].previouseKeyDown && mouseStates[(int)button].isKeyDown;
}

bool Input::GetMouseUp(MounseButton button) const
{
	return mouseStates[(int)button].previouseKeyDown && !mouseStates[(int)button].isKeyDown;
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
	INPUT_RECORD irInBuf[128];
	DWORD cNumRead;

	// 입력 버퍼에 있는 모든 이벤트를 읽어옴
		// PeekConsoleInput의 반환값과 cNumRead를 함께 확인
	if (PeekConsoleInput(hStdin, irInBuf, 128, &cNumRead) && cNumRead > 0)
	{
		// 입력 버퍼에 이벤트가 존재하면, 해당 이벤트를 모두 읽어들임
		// ReadConsoleInput(hStdin, irInBuf, cNumRead, &cNumRead);

		for (DWORD i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
			{
				if (hasFocus)
				{
					keyStates[irInBuf[i].Event.KeyEvent.wVirtualKeyCode].isKeyDown = irInBuf[i].Event.KeyEvent.bKeyDown;
				}
			}
			break;
			case MOUSE_EVENT:
			{
				if (hasFocus)
				{
					HandleMouseEvent(irInBuf[i].Event.MouseEvent);
				}
			}
			break;
			case WINDOW_BUFFER_SIZE_EVENT:
				break;
			case FOCUS_EVENT:
			{
				hasFocus = irInBuf[i].Event.FocusEvent.bSetFocus;

				// 포커스를 잃었을 때 모든 키와 마우스 상태를 초기화
				if (!hasFocus)
				{
					// 키보드 상태 초기화
					for (int j = 0; j < 256; ++j)
					{
						keyStates[j].isKeyDown = false;
						keyStates[j].previouseKeyDown = false;
					}
					// 마우스 상태 초기화
					for (int j = 0; j < 2; ++j)
					{
						mouseStates[j].isKeyDown = false;
						mouseStates[j].previouseKeyDown = false;
					}
				}
			}
			break;
			case MENU_EVENT:
				break;
			default:
				__debugbreak(); // Unknown event type
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

	for (int i = 0; i < 2; ++i)
	{
		mouseStates[i].previouseKeyDown = mouseStates[i].isKeyDown;
	}
}

void Input::HandleMouseEvent(const MOUSE_EVENT_RECORD& mouseEvent)
{
	// 마우스 좌표 업데이트
	mouseX = mouseEvent.dwMousePosition.X;
	mouseY = mouseEvent.dwMousePosition.Y;

	// 마우스 클릭 상태 업데이트
	mouseStates[(int)MounseButton::Left].isKeyDown = (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
	mouseStates[(int)MounseButton::Right].isKeyDown = (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
}
