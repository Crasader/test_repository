#ifndef __War__TutorialScene__
#define __War__TutorialScene__

#include "cocos2d.h"
#include "TitleScene.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "common.h"
#include "HomeCommon.h"
#include "MessageSprite.h"
#include "TouchScreenSprite.h"

#include "UserNameInsertLayer.h"
#include "DeckPackSelectLayer.h"
#include "GeneralSelectLayer.h"
#include "PopupGeneralConfirmLayer.h"
#include "LastConfirmLayer.h"
#include "TutorialGachaLayer.h"
#include "TutorialGachaAnimationLayer.h"
#include "GetGachaLayer.h"
#include "PopupTutorialEndLayer.h"
#include "PopupNameConfirmLayer.h"
#include "SimpleAudioEngine.h"
#include "MessageTalkSprite.h"
#include "BattleTutorialBeforeLayer.h"
#include "BattleTutorialAfterLayer.h"

#define WEI_BUTTON_TAG 1 //魏
#define SHU_BUTTON_TAG 2 //蜀
#define WU_BUTTON_TAG 3 //呉

#define MESSAGE_SPRITE_TAG 10 //瞬き用メッセージ画像のタグ

USING_NS_CC;
using namespace CocosDenshion;

class TutorialScene : public cocos2d::Scene
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(TutorialScene);
    
    void createBg(); //背景画像の作成
    void changePage(Layer* next_layer); //ページの切り替え
    void changePageGachaAnimation(Layer* next_layer); //ガチャアニメーション時のページの切り替え
    void closePopupLayer(Layer* main_layer); //ポップアップを閉じる処理
    int countChildByTag(int Tag) ;
    
    void _setCascadeOpacityEnabled(Node* parent, bool enabled); //再起的に全てに適用させる関数
    
    void blinkTexture(float dt); //画像が瞬きする処理
    void openEyeTexture(float dt); //目が開く画像設定
    void closeEyeTexure(); //目が閉じる画像設定
    
    void cutLeftAndRightVisibleSpace() ; //両サイドの余白に画像を配置する
};

#endif /* defined(__War__TutorialScene__) */
