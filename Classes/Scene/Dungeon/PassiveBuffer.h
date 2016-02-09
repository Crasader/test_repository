
#ifndef __war__PassiveBuffer__
#define __war__PassiveBuffer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

class PassiveBuffer : public Node
{
public:
    CREATE_FUNC(PassiveBuffer);
    
    void initialize();
    void bufferLogic(int used_primary,int buff_id,int value,int index);
    string turnBufferLogic(map<string, int> &buffer);
    string turnHandBufferLogic(Chara *used_chara);
};




#endif /* defined(__war__PassiveBuffer__) */
