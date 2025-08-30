#include "DebugManage.h"


bool DebugManage::isDebugMode = false;
int DebugManage::renderOrder = 100;
DebugManage::Mode DebugManage::mode = DebugManage::Mode::ALL;

int DebugManage::RenderOrder()
{
    return renderOrder;
}

DebugManage::Mode DebugManage::GetMode()
{
    if (isDebugMode)
    {
        return DebugManage::Mode::NONE;
    }

    return mode;
}

void DebugManage::SetMode(Mode val)
{
    mode = val;
}
