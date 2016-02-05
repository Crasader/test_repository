#ifndef __War__TutorialGachaLayer__
#define __War__TutorialGachaLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;

class TutorialGachaLayer : public cocos2d::Layer
{
private:
    float ratio;
    bool _touchFlg; //ボタンタッチフラグ
    
public:
    MessageSprite* message_bg;
    
    virtual bool init();
    CREATE_FUNC(TutorialGachaLayer);
        
    void createMessageBg(); //メッセージ画像の作成
    void createBunner(); //ガチャバナーの作成
    void createButton(); //ボタンの作成
    
    //10連ガチャ通信処理
    void tutorialGachaRequest();
    void tutorialGachaCallback(HttpClient *sender, HttpResponse *response);
    
    //デッキ更新通信処理
    void charaDeckUpdateRequest();
    void charaDeckUpdateCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TutorialGachaLayer__) */
