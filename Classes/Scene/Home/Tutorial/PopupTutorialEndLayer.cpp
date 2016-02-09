#include "PopupTutorialEndLayer.h"

bool PopupTutorialEndLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupTutorialEndLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void PopupTutorialEndLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupTutorialEndLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupTutorialEndLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupTutorialEndLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupTutorialEndLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void PopupTutorialEndLayer::createMessageBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //文字の背景画像
    message_bg = Sprite::create("init/Tutorial/navi1.png");
    message_bg->setScale(0);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(message_bg, 1, POPUP_ACTION_TAG);
}

void PopupTutorialEndLayer::showMessage()
{
    //テキスト
    auto message = HMSFontUtil::createWithTTFBySystemFontArgument("以上でチュートリアル終了です。\nゲームをお楽しみ下さい！", FONT_NAME_SYSTEM, 30);
    message->setHorizontalAlignment(TextHAlignment::LEFT);
    message->setPosition(Point(message_bg->getContentSize().width / 2 + 80, message_bg->getContentSize().height / 2 - 16));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    message_bg->addChild(message, 1);
}

void PopupTutorialEndLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupTutorialEndLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void PopupTutorialEndLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの作成
    createEventListener();
    
    //メッセージ背景の作成
    createMessageBg();
    
    //メッセージの表示
    showMessage();
    
    //ポップアップアクション
    popupAction();

    //BGMの消音
    this->runAction(Sequence::create(FadeToVolumeOfBGM::create(0.1f, 0.0f),
                                     CallFunc::create([](){HMSLOG("volume:%f", SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                    );
}

bool PopupTutorialEndLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupTutorialEndLayer!");
    return true;
}
void PopupTutorialEndLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupTutorialEndLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{    
    //タイトルシーンへの遷移
    auto scene = TitleScene::createScene();
    auto transition = TransitionFade::create(3.0f, scene);
    Director::getInstance()->replaceScene(transition);
}
void PopupTutorialEndLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
