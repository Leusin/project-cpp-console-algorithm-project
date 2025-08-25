#pragma once

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

public:
	Input();
	~Input();

	// 키 처리
	bool GetKey(int KeyCode);
	bool GetKeyDown(int KeyCode);
	bool GetKeyUp(int KeyCode);


	static Input& Get();

private:
	void ProcessInput();
	void SavePreviousKeyStates();

private:
	KeyState keyStates[255] = { };

	static Input* instance;
};
