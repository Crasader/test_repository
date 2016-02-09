#ifndef __War__TeamBattleEventDeckSelectLayer__
#define __War__TeamBattleEventDeckSelectLayer__

#include "cocos2d.h"
#include "DeckChoiceBaseLayer.h"

USING_NS_CC;

class TeamBattleEventDeckSelectLayer : public Layer
{
private:
    DeckChoiceBaseLayer *deck_layer; //デッキ
    
    enum Tag {
        T_Inquiry = 1,
        T_BG,
        T_Deck,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Button,
        Z_Deck,
    };
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TeamBattleEventDeckSelectLayer);
    
    void onEnter(); //初期実行関数
    
    void createButton(); //画面生成
    void createDeck(); //デッキ生成
    void createType(); //タイプ生成
};
#endif /* defined(__War__TeamBattleEventDeckSelectLayer__) */
