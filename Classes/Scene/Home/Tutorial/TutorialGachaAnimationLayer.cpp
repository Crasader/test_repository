#include "TutorialGachaAnimationLayer.h"

bool TutorialGachaAnimationLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TutorialGachaAnimationLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TutorialGachaAnimationLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TutorialGachaAnimationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TutorialGachaAnimationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TutorialGachaAnimationLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TutorialGachaAnimationLayer::showTouchStart()
{
    //タッチ画像の表示
    auto touch_sp = TouchScreenSprite::create();
    touch_sp->setScale(ratio);
    touch_sp->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(touch_sp, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
    
    //イベントリスナーの作成
    createEventLitener();
}

void TutorialGachaAnimationLayer::showEffect(float dt)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile("init/Home/Gacha/gacha_pay.plist");
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames(48);
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 48; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.05f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // 無限ループのアクションを生成
//    auto repeat    = cocos2d::RepeatForever::create(animate);
    auto func = CallFunc::create([this]()
    {
        //タッチスクリーンの表示
        showTouchStart();
    });
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(2.3 * ratio);
    sprite->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    sprite->runAction(Sequence::create(animate, func, nullptr));
    this->addChild(sprite);
}

void TutorialGachaAnimationLayer::onEnter()
{
    Layer::onEnter();
    
    //ガチャエフェクトの表示
    scheduleOnce(schedule_selector(TutorialGachaAnimationLayer::showEffect), 1.2f);
}

bool TutorialGachaAnimationLayer::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("TutorialGachaAnimationLayer! touchBegan");
    return true;
}

void TutorialGachaAnimationLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void TutorialGachaAnimationLayer::onTouchEnded(Touch *touch, Event *event)
{
    //イベントリスナーの削除
    _eventDispatcher->removeEventListener(listener);
    
    auto tutorial_scene = (TutorialScene *)this->getParent();
    auto next_layer = GetGachaLayer::create();
    tutorial_scene->changePage(next_layer);
}
