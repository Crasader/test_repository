#include "MapEscapeConfirmLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsFontUtil.h"

bool MapEscapeConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    return true;
}

void MapEscapeConfirmLayer::initialize()
{
    ratio = getVisibleRatio();
    
}

