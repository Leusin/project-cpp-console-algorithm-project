#pragma once

class DebugManage
{
public:

	enum Mode
	{
		ALL = 0,
		QuadTree,
		Weight,
		Position,
		Path,
		NONE
	};

public:

	void ToggleDebugMode() { isDebugMode = !isDebugMode; }

	static int RenderOrder();

	static Mode GetMode();

	static void SetMode(Mode val);

public:

	static Mode mode;
	static bool isDebugMode;
	static int renderOrder;
};