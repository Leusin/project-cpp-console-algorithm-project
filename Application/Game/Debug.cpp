#include "Debug.h"

int Debug::renderOrder = 100;
bool Debug::IsDebug = false;

bool Debug::IsDebugMode()
{
	return IsDebug;
}

int Debug::RenderOrder()
{
	return renderOrder;
}