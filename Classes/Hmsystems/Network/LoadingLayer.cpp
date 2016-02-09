#include "LoadingLayer.h"

bool LoadingLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("LoadingLayer::init()");

    //モーダルレイヤーにするためのタッチ処理
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(LoadingLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(LoadingLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(LoadingLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(LoadingLayer::onTouchCancelled, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    ratio = getVisibleRatio();
    
    return true;
}

void LoadingLayer::setupImage(float dt)
{
    //背景の影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, ZORDER_LOADING_LAYER, LOADING_LAYER_TAG);
    
    //ローディングの表示
    showEffect("init/Loading/Sprite/loading.plist", convertFromDisplaySize(Point(320, 480), 0.5, 0.5), 1.0f);
    
    //ポップアップアクション
    popupAction();
}

void LoadingLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void LoadingLayer::showEffect(string filename, Vec2 position, float scale)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename);
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(scale);
    sprite->setPosition(position);
    this->addChild(sprite, 10000);
    
    //永続的に発生させる
    auto repeat = RepeatForever::create(animate);
    
    sprite->runAction(repeat);
}

void LoadingLayer::onEnter()
{
    Layer::onEnter();
    
    //画像の表示
    scheduleOnce(schedule_selector(LoadingLayer::setupImage), 1.0f);
}

bool LoadingLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch LoadingLayer!");
    return true;
}
void LoadingLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void LoadingLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void LoadingLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

