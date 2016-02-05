#include "PlusGachaAnimationLayer.h"

bool PlusGachaAnimationLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PlusGachaAnimationLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    
    return true;
}

void PlusGachaAnimationLayer::setData(int gacha_type){
    _gacha_type = gacha_type ;
}

void PlusGachaAnimationLayer::createBg(){
    //タップ禁止レイヤー生成
    AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
    this->addChild(allTapDisaleLayer,1,ALL_TAP_DISABLE_LAYER_TAG);
    
    auto animation_bg = Sprite::create("init/Home/Gacha/back.png");
    animation_bg->setScale(1.0 * ratio,1.2f * ratio);
    animation_bg->setPosition(convertFromDisplaySize(Point(320, visibleSize.height / 2 /ratio), 0.5, 0.5));
    this->addChild(animation_bg, 3);
}

void PlusGachaAnimationLayer::playAnimation(){

    runAction(Sequence::create(DelayTime::create(0.3f),CallFunc::create(CC_CALLBACK_0(PlusGachaAnimationLayer::gachaAnimation,this)),DelayTime::create(1.7f),CallFunc::create(CC_CALLBACK_0(PlusGachaAnimationLayer::quitAnimation,this)), RemoveSelf::create(), nullptr));

    runAction(Sequence::create(DelayTime::create(0.3f),CallFunc::create(CC_CALLBACK_0(PlusGachaAnimationLayer::gachaSE,this)), nullptr));
}

void PlusGachaAnimationLayer::gachaSE(){
    //ガチャアニメーションエフェクト音(１回)
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/boss_erase2.mp3",false);
}

void PlusGachaAnimationLayer::gachaAnimation(){
//    float animation_size = 2.3;
//    if(visibleSize.height >1000)animation_size = 2.7;
    float animation_size = 3.22;
    if(visibleSize.height >1000)animation_size = 3.78;
    

    Effect *animation = Effect::create();
    animation->setScale(1.0*ratio,1.2f*ratio);
    animation->setData(this, "init/Home/Gacha/gacha_plus" , 35, Point((main_screen_w / 2) * ratio,(main_screen_size + header_height + footer_height) / 2 * ratio),animation_size, 10);
}

void PlusGachaAnimationLayer::quitAnimation(){
    this->removeFromParent();
}


void PlusGachaAnimationLayer::onEnter(){
    Layer::onEnter();
    
    //背景画像の作成
    createBg();
    
    //アニメーションの再生
    playAnimation();
    
}
