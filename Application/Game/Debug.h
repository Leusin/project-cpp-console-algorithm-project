#pragma once

class Debug
{
public:
	static bool IsDebugMode();
	static int RenderOrder();

	void ToggleDebugMode() { IsDebug  = !IsDebug; }

private:
	static bool IsDebug;
	static int renderOrder;
};

