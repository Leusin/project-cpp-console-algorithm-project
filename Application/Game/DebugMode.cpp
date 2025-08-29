#include "DebugMode.h"


bool DebugMode::isDebugMode = false;
int DebugMode::renderOrder = 100;

bool DebugMode::IsDebugMode()
{
    return isDebugMode;
}

int DebugMode::RenderOrder()
{
    return renderOrder;
}
