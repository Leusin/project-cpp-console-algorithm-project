#pragma once

class DebugMode
{
public:

	void ToggleDebugMode() { isDebugMode = !isDebugMode; }
	static bool IsDebugMode();
	static int RenderOrder();

private:
	static bool isDebugMode;
	static int renderOrder;
};