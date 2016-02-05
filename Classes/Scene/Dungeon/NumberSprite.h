
#ifndef __war__NumberSprite__
#define __war__NumberSprite__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class NumberSprite : Node
{
public:
    static map<string, vector<Texture2D *>> number_sprite;
    
    static Texture2D* getSprite(int num, string kind, float size);
    
    CREATE_FUNC(NumberSprite);
    
};


#endif /* defined(__war__NumberSprite__) */
