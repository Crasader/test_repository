#include "TapEffectLayer.h"

TapEffectLayer::TapEffectLayer()
{
}

bool TapEffectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TapEffectLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TapEffectLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TapEffectLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TapEffectLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TapEffectLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TapEffectLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//    _eventDispatcher->addEventListenerWithFixedPriority(listener, -1);
}


void TapEffectLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
}

bool TapEffectLayer::onTouchBegan(Touch *touch, Event *event)
{
    if(touch->getID()==0){
        //タップ終了時エフェクトを出す
        showTapEffect(touch->getLocation()) ;
        
        //モーションストリークの生成
//        resetMotionStreak() ;
    }
    
//    //パーティクルを全て削除し、生成
//    for_each(getChildren().begin(), getChildren().end(), [](Node *node){
//        if(node->getTag() == T_TAP_TAIL){
//            node->removeFromParent() ;
//        }
//    }) ;
    
//    // 合成方法: スクリーン
//    BlendFunc blend_linear;
//    blend_linear.src = GL_SRC_ALPHA;
//    blend_linear.dst = GL_ONE;
    
//    ParticleSystemQuad* tap_particle1 = ParticleSystemQuad::create("init/Home/Common/tap_tail.plist");
//    tap_particle1->setAutoRemoveOnFinish(true);
//    tap_particle1->setPosition(touch->getLocation());
//    tap_particle1->setScale(ratio);
//    tap_particle1->setBlendFunc(blend_linear) ;
//    this->addChild(tap_particle1, Z_TAP_TAIL, T_TAP_TAIL);
    
    return true;
}


void TapEffectLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
//    //パーティクルの位置を変える(軌跡が残る)
//    getChildByTag(T_TAP_TAIL)->setPosition(touch->getLocation()) ;
    
    if(touch->getID()==0){
//        addMotionStreakPoint(touch->getLocation()) ;
    }
}

void TapEffectLayer::addMotionStreakPoint(Point point) {
    count_touch_move++ ;
    if (count_touch_move % 5 == 0) {
        MotionStreak* streak = (MotionStreak*)this->getChildByTag(T_MOTION_STREAK);
        streak->setPosition(point);
        int r = rand()%156+100;
        int b = rand()%156+100;
        int g = rand()%156+100;
        streak->setColor(Color3B(r, g, b));
    }
}

void TapEffectLayer::resetMotionStreak() {
    this->removeChildByTag(T_MOTION_STREAK, true);
    MotionStreak* streak = MotionStreak::create(0.5f, 0.05, 30, Color3B(255, 255, 255), "init/Home/Common/tap_tail_motion_streak.png");
    streak->setBlendFunc(BlendFunc::ADDITIVE) ;
    this->addChild(streak, 100, T_MOTION_STREAK);
}


void TapEffectLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TapEffectLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TapEffectLayer::showTapEffect(Point point)
{
    // 合成方法: スクリーン
    BlendFunc blend_screen;
    blend_screen.src = GL_ONE_MINUS_DST_COLOR;
    blend_screen.dst = GL_ONE;

    /////////////////////tapエフェクト////////////////////////
    Sprite* tap_center = Sprite::create("init/Home/Common/tap_center.png") ;
    tap_center->setPosition(point) ;
    tap_center->setScale(1*ratio) ;
    tap_center->runAction(Sequence::create(Spawn::create(FadeOut::create(0.2f), EaseIn::create(ScaleTo::create(0.2f, 2*ratio), 1), nullptr), RemoveSelf::create(), nullptr)) ;
    tap_center->setBlendFunc(blend_screen) ;
    this->addChild(tap_center, Z_TAP_CENTER, T_TAP_CENTER) ;

    Sprite* tap_waveform = Sprite::create("init/Home/Common/tap_waveform.png") ;
    tap_waveform->setScale(0.0f) ;
    tap_waveform->setPosition(point) ;
    tap_waveform->runAction(Sequence::create(Spawn::create(EaseIn::create(FadeOut::create(0.2f), 8), ScaleTo::create(0.2f, 1.5*ratio), nullptr), RemoveSelf::create(), nullptr)) ;
    tap_waveform->setBlendFunc(blend_screen) ;
    this->addChild(tap_waveform, Z_TAP_WAVEFORM, T_TAP_WAVEFORM) ;
    
}
