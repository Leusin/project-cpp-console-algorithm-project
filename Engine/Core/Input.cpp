#include "Input.h"

#include <Windows.h>

Input* Input::instance = nullptr;

Input::Input()
{
	instance = this; // 싱글턴 인스턴스 변수 설정
}

Input::~Input()
{
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

Input& Input::Get()
{
	return *instance;
}

void Input::ProcessInput()
{
	// 키입력 확인
	for (int keyCode = 0; keyCode < 255; ++keyCode)
	{
		keyStates[keyCode].isKeyDown = GetAsyncKeyState(keyCode) & 0x80000;
	}
}

void Input::SavePreviousKeyStates()
{
	for (int keyCode = 0; keyCode < 255; ++keyCode)
	{
		keyStates[keyCode].previouseKeyDown = keyStates[keyCode].isKeyDown;
	}
}
