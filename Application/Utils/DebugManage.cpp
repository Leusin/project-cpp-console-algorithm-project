#include "DebugManage.h"


bool DebugManage::isDebugMode = false;
int DebugManage::renderOrder = 100;

bool DebugManage::IsDebugMode()
{
    return isDebugMode;
}

int DebugManage::RenderOrder()
{
    return renderOrder;
}
