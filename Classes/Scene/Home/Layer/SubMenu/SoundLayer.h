#ifndef __war__SoundLayer__
#define __war__SoundLayer__

#include <stdio.h>

#include "cocos2d.h"
#include "cocos-ext.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "ModalLayer.h"
#include "NativeBridge.h"
#include "SimpleAudioEngine.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "network/HttpClient.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define CHANGE_EFFECT_TAG 88 //エフェクトのチェンジタグ
#define SOUND_SLIDER 11      //BGM音量スライダー
#define EFFECT_SLIDER 12     //効果音量スライダー
#define EFFECT_ON 13         //エフェクトオンボタン
#define EFFECT_OFF 14        //エフェクトオフボタン
#define EFFECT_ON_GRAY 17    //エフェクトオングレーボタン
#define EFFECT_OFF_GRAY 18   //エフェクトオフグレーボタン
#define SOUND_DEFAULT 15     //音量デフォルトボタン
#define SOUND_SET 16         //音量適用ボタン

USING_NS_CC;
using namespace cocos2d::extension;

class SoundLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
protected:
    enum btnMenuZOrder {
        Z_button = 1,
        Z_label,
    };
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(SoundLayer);
    
    //画像の設定
    void setupImage();
    
    void soundPage(Ref* sender);
    
    void changePage(Ref* sender);
    
    void changeVolume(Ref* sender);
    
    void changeBgmVolume(Ref* sender, Control::EventType controlEvent);
    
    void changeSeVolume(Ref* sender, Control::EventType controlEvent);
    
    void changeEffect(Ref* sender);
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void valueChanged(Node *pSender,Control::EventType pControlEvent);
    
    void onEnter();
};

#endif /* defined(__war__SoundLayer__) */
