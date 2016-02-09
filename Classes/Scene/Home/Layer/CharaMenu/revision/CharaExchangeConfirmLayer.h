#ifndef __war__CharaExchangeConfirmLayer__
#define __war__CharaExchangeConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaExchangeConfirmSprite.h"

USING_NS_CC;

class CharaExchangeConfirmLayer : public Layer
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
    
    CharaExchangeConfirmSprite* _chara_confirm_bg; //上部キャラ画像の背景画像
    Sprite* _bg; //下部はい、いいえ背景画像
    Json _chara_data; //キャラデータ
    
public:
    CharaExchangeConfirmLayer(); //コンストラクタ
    
    float ratio;
    static CharaExchangeConfirmLayer* create(Json chara_data);
    virtual bool init(Json chara_data);
    //    CREATE_FUNC(CharaExchangeConfirmLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createConfirmLabel(); //確認文字列
    void createButton(); //はい、いいえボタン
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaExchangeConfirmLayer__) */
