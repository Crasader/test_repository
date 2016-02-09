#include "TitleBarLayer.h"

bool TitleBarLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TitleBarLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}
