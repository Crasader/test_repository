#ifndef __war__OptionsLayer__
#define __war__OptionsLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "NicknameLayer.h"
#include "NativeBridge.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define SOUND_TAG 11       //サウンド設定タグ
#define NICKNAME_TAG 12    //ニックネーム変更タグ
#define PRODUCTION_TAG 13  //演出設定タグ

USING_NS_CC;

class OptionsLayer : public Layer
{
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(OptionsLayer);
    
    //画像の設定
    void setupImage();
    
    void changePage(Ref* sender);
    
    void onEnter();

};
#endif /* defined(__war__OptionsLayer__) */
