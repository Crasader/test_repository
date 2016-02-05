#ifndef __war__BazaarPurchaseConfirmLayer__
#define __war__BazaarPurchaseConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaExchangeConfirmSprite.h"

USING_NS_CC;

class BazaarPurchaseConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Menu,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Menu,
    };
    
    //CharaExchangeConfirmSprite* chara_confirm_bg; //上部キャラ画像の背景画像
    Sprite* background; //下部はい、いいえ背景画像
    Json charaData; //キャラデータ
    
public:
    BazaarPurchaseConfirmLayer(); //コンストラクタ
    
    Node* node; //親レイヤーのポインタを保持しておく
    float ratio;
    static BazaarPurchaseConfirmLayer* create(Json charaData, Node* node);
    virtual bool init(Json charaData, Node* node);

    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createConfirmLabel(); //確認文字列
    void createButton(); //はい、いいえボタン
    void createCardDetailLayer();
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__BazaarPurchaseConfirmLayer__) */
