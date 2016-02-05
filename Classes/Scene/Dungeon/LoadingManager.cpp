
#include "LoadingManager.h"
#include "PlistAnimationSprite.h"

LoadingManager* LoadingManager::loading_manager = nullptr;//これを入れないとエラーに・・・

bool LoadingManager::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("LoadingManager::init()");
    
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    ratio = getVisibleRatio();
    
    setCascadeOpacityEnabled(true);//下の画像にもopacity適用
    return true;
}


LoadingManager* LoadingManager::setLoading(Layer *layer, string option){
    
    if(loading_manager != nullptr && loading_manager->isRunning()){
        loading_manager->removeFromParentAndCleanup(true);
        loading_manager = nullptr;
    }
    loading_manager = LoadingManager::create();
    loading_manager->listener->setSwallowTouches(true);
    layer->addChild(loading_manager,10000000);
    loading_manager->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5, 0.5) - loading_manager->convertToWorldSpaceAR(Size::ZERO));
    
    if(option == "clear"){
        
    }else{
        //背景の影
        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
        shadow->setScale(loading_manager->ratio);
        shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        //shadow->setOpacity(0);
        loading_manager->addChild(shadow, ZORDER_LOADING_LAYER, LOADING_LAYER_TAG);
    }
    loading_manager->setOpacity(0);
    loading_manager->runAction(Sequence::create(DelayTime::create(0.2f), FadeTo::create(0.4f, 255), NULL));
    
    //ローディングの表示
    //loading_manager->showEffect("init/Loading/Sprite/loading.plist", convertFromDisplaySize(Point(320, 480), 0.5, 0.5), loading_manager->ratio);
    
    auto sprite2 = PlistAnimationSprite::setData(loading_manager, "init/Loading/Sprite/loading", 0, 0.06f);
    sprite2->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    sprite2->setScale(loading_manager->ratio);
    
    
    return loading_manager;
}

/*
void LoadingManager::showEffect(string filename, Vec2 position, float scale)
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
    //sprite->setOpacity(0);
    this->addChild(sprite, 10000);
    //sprite->runAction(FadeTo::create(0.4f, 255));
    
    //永続的に発生させる
    auto repeat = RepeatForever::create(animate);
    
    sprite->runAction(repeat);
}*/

void LoadingManager::removeLoading(){
    if(loading_manager != nullptr && loading_manager->isRunning()){
        //loading_manager->stopAllActions();
        loading_manager->listener->setSwallowTouches(false);
        loading_manager->runAction(Sequence::create(FadeOut::create(0.4f), CallFunc::create([&](){
            loading_manager->removeFromParentAndCleanup(true);
            loading_manager = nullptr;
        }), NULL));
    }
}
void LoadingManager::onExit(){
    Layer::onExit();
    loading_manager = nullptr;
}
