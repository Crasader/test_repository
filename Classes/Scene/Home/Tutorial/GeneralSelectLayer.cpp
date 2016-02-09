#include "GeneralSelectLayer.h"

bool GeneralSelectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("GeneralSelectLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void GeneralSelectLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GeneralSelectLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GeneralSelectLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GeneralSelectLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GeneralSelectLayer::setupImage()
{
    //豪傑デッキ画像
    auto gouketu_deck = Sprite::create("init/Tutorial/bt_select1.png");
    gouketu_deck->setScale(ratio);
    gouketu_deck->setPosition(convertFromDisplaySize(Point(320, 720), 0.5, 0.5));
    this->addChild(gouketu_deck, 1, 1);
    
    //知略デッキ画像
    auto tiryaku_deck = Sprite::create("init/Tutorial/bt_select2.png");
    tiryaku_deck->setScale(ratio);
    tiryaku_deck->setPosition(convertFromDisplaySize(Point(320, 540), 0.5, 0.5));
    this->addChild(tiryaku_deck, 1, 2);
    
    //非道デッキ画像
    auto hidou_deck = Sprite::create("init/Tutorial/bt_select3.png");
    hidou_deck->setScale(ratio);
    hidou_deck->setPosition(convertFromDisplaySize(Point(320, 360), 0.5, 0.5));
    this->addChild(hidou_deck, 1, 3);
    
    //美女と猛将デッキ画像
    auto bizyo_deck = Sprite::create("init/Tutorial/bt_select3.png");
    bizyo_deck->setScale(ratio);
    bizyo_deck->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
    this->addChild(bizyo_deck, 1, 4);
    
    //南蛮デッキ画像
    auto nanban_deck = Sprite::create("init/Tutorial/bt_select3.png");
    nanban_deck->setScale(ratio);
    nanban_deck->setPosition(convertFromDisplaySize(Point(320, 0), 0.5, 0.5));
    this->addChild(nanban_deck, 1, 5);
}

void GeneralSelectLayer::createMessageBg()
{
    //文字画像の作成
    message_bg = MessageSprite::create("最初にデッキを１つ\n選んでね", TextHAlignment::LEFT);
    message_bg->setScale(ratio);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 900), 0.5, 0.5));
    addChild(message_bg, 1, MESSAGE_SPRITE_TAG);
}

bool GeneralSelectLayer::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("GeneralSelectLayer! touchBegan");
    
    auto children = this->getChildren();
    for (auto child : children)
    {
        auto sp = (Sprite *)child;
        Rect rect = sp->getBoundingBox();
        
        if (rect.containsPoint(touch->getLocation()) && sp->getTag() > 0)
        {
            //メンバ変数に保存
            _touchSprite = sp;
            _touchSprite->setScale(1.2 * ratio);
            _touchFlg = true;
        }
    }
    
    return true;
}

void GeneralSelectLayer::onTouchMoved(Touch *touch, Event *event)
{
    Rect rect = _touchSprite->getBoundingBox();
    if (!rect.containsPoint(touch->getLocation()))
    {
        _touchSprite->setScale(1.0 * ratio);
        _touchFlg = false;
    }
}

void GeneralSelectLayer::onTouchEnded(Touch *touch, Event *event)
{
    if (_touchFlg)
    {
        _touchSprite->setScale(1.0 * ratio);
        _touchFlg = false;
        
        //ポップアップにも表示させるのでメッセージ画像を見えなくさせる
        message_bg->setVisible(false);
        
        auto popup_layer = PopupGeneralConfirmLayer::create(toString<int>(_touchSprite->getTag()));
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        HMSLOG("tag = %d", _touchSprite->getTag());
    }
}

void GeneralSelectLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの作成
    createEventLitener();
    
    //メッセージ背景の作成
    createMessageBg();
    
    //画像の作成
    setupImage();
}
