#ifndef __war__EvolutionAnimationLayer__
#define __war__EvolutionAnimationLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "GachaMenuLayer.h"
#include "network/HttpClient.h"
#include "Effect.h"
#include "SuperAnimNodeV2.h"

class EvolutionAnimationLayer : public Layer
{
private:
public:
    EventListenerTouchOneByOne *listener;
    float ratio;
    Size visibleSize;
    float header_height;//ヘッダーの高さ
    float footer_height;//フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;
    
    bool animation_finish_flg = false;
    
    string _chara_id;
    string _next_chara_id;
    string _next_chara_list_id;
    Sprite *img0;
    Sprite *img1;
    Sprite *img2;
    Sprite *effect_sprite;
    Sprite *white_sprite;
    
    virtual bool init();
    CREATE_FUNC(EvolutionAnimationLayer);
    
    void initEventListener(); //イベントリスナーの設定
    
    void setData(string chara_id, string next_chara_id, string next_chara_list_id);
    
    void createBg();//背景画像の作成
    
    void playAnimation();//アニメーションの再生
    void playEffect();
    void playAnimation2();
    void playAnimation3();
    void playAnimation4();
    void playAnimation5();
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    //初期実行関数
    void onEnter();
};
#endif /* defined(__war__EvolutionAnimationLayer__) */
