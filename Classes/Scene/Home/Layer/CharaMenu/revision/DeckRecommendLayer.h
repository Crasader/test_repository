#ifndef __war__DeckRecommendLayer__
#define __war__DeckRecommendLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"

USING_NS_CC;

class DeckRecommendLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Bg,
        T_Title,
        T_Close_Menu,
        T_Offense_Deck,
        T_Defense_Deck,
        T_Strategist_Deck,
        T_Deck_Menu,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Bg,
        Z_Title,
        Z_Close_Menu,
        Z_Offense_Deck,
        Z_Defense_Deck,
        Z_Strategist_Deck,
        Z_Deck_Menu,
    };
    
    Sprite* _bg; //背景画像
    
public:
    DeckRecommendLayer(); //コンストラクタ
    
    float ratio;
    static DeckRecommendLayer* create();
    virtual bool init();
//    CREATE_FUNC(DeckRecommendLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //はい、いいえボタンの作成
    
    void selectedDeckType(Ref* sender); //おすすめデッキタイプのコールバック
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckRecommendLayer__) */
