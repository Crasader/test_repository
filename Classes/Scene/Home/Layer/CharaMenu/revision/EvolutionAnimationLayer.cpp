#include "EvolutionAnimationLayer.h"

bool EvolutionAnimationLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("EvolutionAnimationLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    
    return true;
}

void EvolutionAnimationLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(EvolutionAnimationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EvolutionAnimationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EvolutionAnimationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(EvolutionAnimationLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void EvolutionAnimationLayer::setData(string chara_id, string next_chara_id, string next_chara_list_id){
    _chara_id = chara_id;
    _next_chara_id = next_chara_id;
    _next_chara_list_id = next_chara_list_id;
    
    playAnimation();
}

void EvolutionAnimationLayer::createBg(){
    //タップ禁止レイヤー生成
    AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
    this->addChild(allTapDisaleLayer,1,ALL_TAP_DISABLE_LAYER_TAG);
    
    auto animation_bg = Sprite::create("init/Home/Gacha/back2.png");
    animation_bg->setScale(1.0 * ratio,1.2f * ratio);
    animation_bg->setPosition(convertFromDisplaySize(Point(320, visibleSize.height / 2 /ratio), 0.5, 0.5));
    this->addChild(animation_bg, 3);
}

void EvolutionAnimationLayer::playAnimation(){
    
    /*
runAction(Sequence::create(DelayTime::create(0.3f),CallFunc::create(this,callfunc_selector(EvolutionAnimationLayer::gachaAnimation)),DelayTime::create(1.7f),CallFunc::create(this,callfunc_selector(EvolutionAnimationLayer::quitAnimation)), RemoveSelf::create(), nullptr));
    
runAction(Sequence::create(DelayTime::create(0.3f),CallFunc::create(this,callfunc_selector(EvolutionAnimationLayer::gachaSE)), nullptr));
     */
    
    int frame1 = 25;
    int frame2 = 105;
    
    //合成前キャラのアニメーション
    img0 = Sprite::create("download/Chara/all400/" + toString(_chara_id) + ".png");
    img0->setScale(0.7*ratio);
    img0->setPosition(convertFromDisplaySize(Point(320, 480+100), 0.5, 0.5));
    this->addChild(img0, 11);
    
    MoveTo *move0 = MoveTo::create(frame1*0.05f, convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    CallFunc *func = CallFunc::create(CC_CALLBACK_0(EvolutionAnimationLayer::playEffect,this));
    img0->runAction(Sequence::create(move0, func, nullptr));
    
    img1 = Sprite::create("download/Chara/all400/" + toString(_chara_id) + ".png");
    img1->setScale(0.7*ratio);
    img1->setPosition(convertFromDisplaySize(Point(320+460, 480), 0.5, 0.5));
    this->addChild(img1, 10);
    
    DelayTime *delay1 = DelayTime::create(frame1*0.05f);
    MoveTo *move1 = MoveTo::create(4.0f, convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    CallFunc *func1 = CallFunc::create(CC_CALLBACK_0(EvolutionAnimationLayer::playAnimation2,this));
    img1->runAction(Sequence::create(delay1, move1, func1, nullptr));
    
    img2 = Sprite::create("download/Chara/all400/" + toString(_chara_id) + ".png");
    img2->setScale(0.7*ratio);
    img2->setPosition(convertFromDisplaySize(Point(320-460, 480), 0.5, 0.5));
    this->addChild(img2, 10);
    
    DelayTime *delay2 = DelayTime::create(frame1*0.05f);
    MoveTo *move2 = MoveTo::create(4.0f, convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    img2->runAction(Sequence::create(delay2, move2, nullptr));
}

void EvolutionAnimationLayer::playEffect() {
    //イフェクト
    string filename = "init/CharaMenu/evolution";
    float frame_rate = 0.04f;
    int z_order = 20;
    int loop_count = 10000;
    Point effect_position = Point((main_screen_w / 2) * ratio,(main_screen_size + header_height + footer_height) / 2 * ratio);

    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename + ".plist", filename + ".png");
    
    auto anim = Animation::create();
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    char str[100] = {0};
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(frame_rate);
    anim->setRestoreOriginalFrame(true);
    
    effect_sprite = Sprite::createWithSpriteFrameName("01.png");//最初の画像
    effect_sprite->setPosition(effect_position);
    effect_sprite->setScale(3.2*ratio);
    this->addChild(effect_sprite, z_order);
    
    auto action = Animate::create(anim);
    
    
    if(loop_count == 0){//0の場合は無限ループ
        auto repeat = RepeatForever::create(action);
        effect_sprite->runAction(repeat);
    }else{//1以上の場合は回数分再生
        auto repeat = Repeat::create(action,loop_count);
        effect_sprite->runAction(Sequence::create(repeat, RemoveSelf::create(), NULL));
    }
    
    BlendFunc blend;//加算合成
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    effect_sprite->setBlendFunc(blend);
}

void EvolutionAnimationLayer::playAnimation2()
{
    HMSLOG("playAnimation2");
    this->removeChild(img1);
    this->removeChild(img2);
    //this->removeChild(effect_sprite);
    
    //白い画像をフェードインさせる
    //Blink *blink = Blink::create(1.0f, 1);
    //CallFunc *func = CallFunc::create(this, callfunc_selector(EvolutionAnimationLayer::playAnimation3));
    //img0->runAction(Sequence::create(blink, func, nullptr));
    white_sprite = Sprite::create("init/CharaMenu/card_white.png");
    white_sprite->setScale(0.7*ratio);
    white_sprite->setOpacity(0);
    white_sprite->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(white_sprite, 11);

    FadeIn *fadein = FadeIn::create(1.0f);
    CallFunc *func = CallFunc::create(CC_CALLBACK_0(EvolutionAnimationLayer::playAnimation3,this));
    white_sprite->runAction(Sequence::create(fadein, func, nullptr));
}

void EvolutionAnimationLayer::playAnimation3()
{
    HMSLOG("playAnimation3");
    this->removeChild(img0);
    this->removeChild(white_sprite);
    this->removeChild(effect_sprite);
    
    //合成後キャラのアニメーション
    img0 = Sprite::create("download/Chara/all400/" + toString(_next_chara_id) + ".png");
    img0->setScale(0.7*ratio*0.2);
    img0->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(img0, 11);
    
    ScaleTo *scale = ScaleTo::create(12*0.04f, ratio);
    DelayTime *delay = DelayTime::create(13*0.04f);
    CallFunc *func = CallFunc::create(CC_CALLBACK_0(EvolutionAnimationLayer::playAnimation4,this));
    img0->runAction(Sequence::create(scale, delay, func, nullptr));
}

void EvolutionAnimationLayer::playAnimation4()
{
    HMSLOG("playAnimation4");
    float span = 5*0.04f;
    ScaleTo *scale = ScaleTo::create(span, 0.8*ratio);
    MoveTo *move = MoveTo::create(span, convertFromDisplaySize(Point(320, 480+200*0.8+10), 0.5, 0.5));
    Spawn *action = Spawn::create(scale, move, nullptr);
    CallFunc *func = CallFunc::create(CC_CALLBACK_0(EvolutionAnimationLayer::playAnimation5,this));
    img0->runAction(Sequence::create(action, func, nullptr));
    
    CharaDetailSpriteForResultAnimation *_chara_detail_sprite = CharaDetailSpriteForResultAnimation::create(fromString<int>(_next_chara_list_id));
    _chara_detail_sprite->setScale(0);
    _chara_detail_sprite->setPosition(convertFromDisplaySize(Point(320, 260), 0.5, 0.5));
    
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
    
    auto scale2 = ScaleTo::create(span, 1.0*ratio);
    _chara_detail_sprite->runAction(scale2);
    this->addChild(_chara_detail_sprite,500);
}

void EvolutionAnimationLayer::playAnimation5()
{
    HMSLOG("playAnimation5");
    animation_finish_flg = true;
    
    this->removeChildByTag(ALL_TAP_DISABLE_LAYER_TAG);
    //this->removeFromParent();
}

/*
void EvolutionAnimationLayer::gachaSE(){
    //ガチャアニメーションエフェクト音(１回)
    //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/boss_erase2.mp3",false);
}

void EvolutionAnimationLayer::gachaAnimation(){
//    float animation_size = 2.3;
//    if(visibleSize.height >1000)animation_size = 2.7;
    float animation_size = 3.22;
    if(visibleSize.height >1000)animation_size = 3.78;
    
    Effect *animation = Effect::create();
    animation->setScale(1.0*ratio,1.2f*ratio);
    animation->setData(this, "init/Home/Gacha/gacha_free" , 35, Point((main_screen_w / 2) * ratio,(main_screen_size + header_height + footer_height) / 2 * ratio),animation_size, 10);
}

void EvolutionAnimationLayer::quitAnimation(){
    this->removeFromParent();
}
*/

bool EvolutionAnimationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaEvolutionAnimationLayer!");
    
    if(animation_finish_flg==true) {
        //this->removeFromParent();
        auto normal_scene = (NormalScene *)this->getParent();
        //キャラ一覧へ
        normal_scene->changePage("CharaMenu");
        
        this->removeFromParent();
    }
    
    return true;
}
void EvolutionAnimationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void EvolutionAnimationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void EvolutionAnimationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void EvolutionAnimationLayer::onEnter(){
    Layer::onEnter();
    
    initEventListener();
    
    //背景画像の作成
    createBg();
}
