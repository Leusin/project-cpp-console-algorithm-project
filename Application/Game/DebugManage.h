#pragma once

class DebugManage
{
public:

	enum Mode
	{
		ALL = 0,
		QuadTree,
		Map,
		Weight,
		SIZE
	};

public:

	void ToggleDebugMode() { isDebugMode = !isDebugMode; }
	static bool IsDebugMode();
	static int RenderOrder();

public:

	Mode mode = Mode::ALL;

private:

	static bool isDebugMode;
	static int renderOrder;
};