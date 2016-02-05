#ifndef __war__TeamBattleEventItemExchangeSprite__
#define __war__TeamBattleEventItemExchangeSprite__

#include "ItemExchangeSpriteBase.h"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class TeamBattleEventItemExchangeSprite : public ItemExchangeSpriteBase
{
public:
    static TeamBattleEventItemExchangeSprite* create(json11::Json json, Node* normal_scene);
    
    void exchangeRequest();
    void exchangeRequestCallback(HttpClient *sender, HttpResponse *response);
    
    //override
    void tapExchangeButtonAction(); //交換ボタンタップ時のアクション
    void setExchangeItemData(); //交換に使用するアイテム画像のパスのセット
};

#endif /* defined(__war__TeamBattleEventItemExchangeSprite__) */
