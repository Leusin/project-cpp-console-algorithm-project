#pragma once

#include <Windows.h>
#include "Core.h"

enum class MounseButton
{
	Left = 0,
	Right = 1
};

class Engine_API Input
{
	friend class Engine;

	/// <summary>
	/// 키입력 확인을 위한
	/// </summary>
	struct KeyState
	{
		bool isKeyDown = false; // 현재 프레임에 키가 눌렸는지
		bool previouseKeyDown = false; // 이전 프레임에 키가 눌렸는지 여부
	};

public: // RTTI

	Input();
	~Input();

public: // GET SET

	// 키 처리
	bool GetKey(int KeyCode) const;
	bool GetKeyDown(int KeyCode) const;
	bool GetKeyUp(int KeyCode) const;

	// 마우스 처리
	bool GetMouse(MounseButton button) const;
	bool GetMouseDown(MounseButton button) const;
	bool GetMouseUp(MounseButton button) const;

	int GetMouseX() const;
	int GetMouseY() const;

	static Input& Get();

private: // METHOD

	void ProcessInput();
	void SavePreviousKeyStates();
	void HandleMouseEvent(const MOUSE_EVENT_RECORD& mouseEvent);

private: // FIELD

	// 키보드 상태 배열
	KeyState keyStates[256] = { };

	// 마우스 상태
	KeyState mouseStates [2];
	int mouseX = 0;
	int mouseY = 0;

	static Input* instance;

	HANDLE hStdin;

	// 콘솔 기존 입력 모드
	DWORD fdwSaveOldMode;

	// 윈도우 포커스 상태를 나타내는 변수
	bool hasFocus = true;
};
