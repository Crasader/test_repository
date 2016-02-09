#ifndef __war__MapEscapeConfirmLayer__
#define __war__MapEscapeConfirmLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "MapCharaLayer.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std;

class MapEscapeConfirmLayer : public Layer
{
private:
    float ratio;
public:
    virtual bool init();
    CREATE_FUNC(MapEscapeConfirmLayer);
    
    void initialize();
};

#endif /* defined(__war__MapEscapeConfirmLayer__) */
