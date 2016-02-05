#include "BattleTutorialAfterLayer.h"

bool BattleTutorialAfterLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleTutorialAfterLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BattleTutorialAfterLayer::createMessageBg()
{
    //文字の背景画像
    message_bg = MessageSprite::create("チュートリアル、お疲れ様でした！\nゲームを始めますので\nスターターデッキを選んで下さい♪", TextHAlignment::LEFT);
    message_bg->setScale(ratio);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 660), 0.5, 0.5));
    addChild(message_bg, 1, MESSAGE_SPRITE_TAG);
}


void BattleTutorialAfterLayer::createButton()
{
    //初回10連ガチャを引くボタン
    auto btn_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto btn_on  = Sprite::create("init/Home/Common/bt_ok.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto btn = MenuItemSprite::create(btn_off, btn_on, [this](Ref* sender)
    {
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");

        //ページ遷移
        auto tutorial_scene = (TutorialScene *)this->getParent();
        auto next_layer = DeckPackSelectLayer::create();
        tutorial_scene->changePage(next_layer);
    });
    
    btn->setScale(1.2 * ratio);
    auto Menu = Menu::create(btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(320, DISPLAY_HEIGHT/2), 0.5, 0.5));
    addChild(Menu, 1);
}


void BattleTutorialAfterLayer::onEnter()
{
    Layer::onEnter();
    
    //メッセージ画像の背景作成
    createMessageBg();
    
    //ボタンの作成
    createButton();
}
