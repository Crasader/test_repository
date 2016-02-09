
#ifndef __war__ActiveBuffer__
#define __war__ActiveBuffer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"

USING_NS_CC;

class ActiveBuffer : public Node
{
public:
    CREATE_FUNC(ActiveBuffer);
    
    void initialize();
    void bufferLogic(Chara* chara, map<string, int> buffer);
    string turnBufferLogic(Chara *chara, map<string, int> &buffer);
};

#endif /* defined(__war__ActiveBuffer__) */
