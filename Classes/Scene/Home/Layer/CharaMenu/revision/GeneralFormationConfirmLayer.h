#ifndef __war__GeneralFormationConfirmLayer__
#define __war__GeneralFormationConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"

#define POS_X_SELL_FIRST 63
#define POS_Y_SELL_FIRST 340
#define POS_X_SELL_DISTANCE 97
#define POS_Y_SELL_DISTANCE 96

USING_NS_CC;

class GeneralFormationConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title,
        T_Close_Menu,
        T_Chara_Bg,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title,
        Z_Close_Menu,
        Z_Chara_Bg,
    };
    
    Sprite* _bg; //背景画像
    HmsVector<CharaSprite *> _chara; //確認総大将
    CharaSprite* _chara_data;
    
public:
    GeneralFormationConfirmLayer(); //コンストラクタ
    
    float ratio;
    static GeneralFormationConfirmLayer* create(CharaSprite* chara);
    virtual bool init(CharaSprite* chara);
    //    CREATE_FUNC(GeneralFormationConfirmLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //はい、いいえボタンの作成
    void createGeneralChara(); //キャラの作成
    Json createCharaJsonData(); //キャラのJsonデータの作成
    void showGeneralChara(); //総大将キャラの表示
    
    //総大将変更通信処理
    void generalCharaDataRequest();
    void generalCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__GeneralFormationConfirmLayer__) */
