//画像や文字にアウトラインを付けてスプライトで利用するクラス

#ifndef __war__OutlineSprite__
#define __war__OutlineSprite__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class OutlineSprite : Sprite
{
public:
    Sprite *sprite;
    Sprite *outline;
    Label *label;
    
    static Sprite* createWithSprite(Sprite *sprite, float outline_size);
    static Sprite* createWithLabel(Label *Label, float outline_size);
    static Texture2D* createWithLabelOneSprite(Label *Label, float outline_size);
    CREATE_FUNC(OutlineSprite);

};
#endif /* defined(__war__OutlineSprite__) */
