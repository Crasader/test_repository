
#ifndef __war__PassiveSkill__
#define __war__PassiveSkill__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std;

class PassiveSkill : public Node
{
public:
    CREATE_FUNC(PassiveSkill);
    
    void initialize();
    string skillLogic(int used_primary);
    Sprite* getIcon(int skill_id);
    int reverseIndex(int index);
};






#endif /* defined(__war__PassiveSkill__) */
