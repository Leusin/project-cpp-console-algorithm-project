#pragma once

#include <Windows.h>
#include "Core.h"


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

public: // MESSAGE

	// 키 처리
	bool GetKey(int KeyCode);
	bool GetKeyDown(int KeyCode);
	bool GetKeyUp(int KeyCode);

	// 마우스 처리
	bool GetMouseClick();
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
	bool isMouseClick = false;
	int mouseX = 0;
	int mouseY = 0;

	static Input* instance;

	HANDLE hInput;
};
