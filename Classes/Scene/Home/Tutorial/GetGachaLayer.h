#ifndef __War__GetGachaLayer__
#define __War__GetGachaLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

#define CARD_1_POS_X 80
#define CARD_1_POS_Y 500
#define CARD_DISTANCE_X 120
#define CARD_DISTANCE_Y 140

USING_NS_CC;
using namespace network;
using namespace json11;

class GetGachaLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    Json _gacha_result; //ガチャ結果
    HmsVector<Sprite *> _card; //カード
    MessageSprite* message_bg;
    
    int _card_num; //カードの番号
    bool _action_flg; //アクション中か判定するフラグ
    
public:
    float ratio;
    GetGachaLayer(); //コンストラクタ

    virtual bool init();
    CREATE_FUNC(GetGachaLayer);
    
    void createEventLitener(); //イベントリスナーの作成
    void setupGachaResult(); //ガチャ結果の読み込み
    void createCards(); //初期カードの作成
    void initCards(); //カードの配置
    
    void createMessage(); //メッセージの作成
    
    void showGetCard(float dt); //手に入れたカードの表示
    void showGetCardAll(); //全てのカードを表示
    
    void showTouchStart(); //タッチ画像の表示

    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__GetGachaLayer__) */
