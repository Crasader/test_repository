#ifndef __war__GachaAnimationLayer__
#define __war__GachaAnimationLayer__

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

class GachaAnimationLayer : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener
{
private:
    SuperAnim::SuperAnimNode* mAnimNode;
public:
    float ratio;
    Size visibleSize;
    float header_height;//ヘッダーの高さ
    float footer_height;//フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;
    int _gacha_type ;

    virtual bool init();
    CREATE_FUNC(GachaAnimationLayer);
    
    void setData(int gacha_type) ;
    
    void createBg();//背景画像の作成
    
    void playAnimation();//アニメーションの再生

    void gachaSE() ; //ガチャのSE
    void gachaAnimation();//ガチャアニメーションエフェクト処理

    void quitAnimation();//レイヤーを閉じる

    //初期実行関数
    void onEnter();
};
#endif /* defined(__war__GachaAnimationLayer__) */
