
#ifndef __war__ContinueStoreScroll__
#define __war__ContinueStoreScroll__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "NormalScene.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ContinueStoreScroll : public ScrollView
{
private:
    float ratio;
    float _cell_size;
    
public:
    void onEnter();
    void setJson(Json json_txt);
    void makeJewelSprite(string name,string price,string name_id, int line);
    
    CREATE_FUNC(ContinueStoreScroll);	//create関数
    
    EventListenerTouchOneByOne *listener;
};


#endif /* defined(__war__ContinueStoreScroll__) */
