#include "TournamentStartAnimationLayer.hpp"

#define DEFAULT_POS 84

TournamentStartAnimationLayer* TournamentStartAnimationLayer::create(Node* node)
{
    TournamentStartAnimationLayer *sprite = new TournamentStartAnimationLayer();
    
    if (sprite && sprite->init(node))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentStartAnimationLayer::init(Node* node)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentStartAnimationLayer::init()");

    ratio = getVisibleRatio();
    
    _node = node;
    
    return true;
}

void TournamentStartAnimationLayer::onEnter()
{
    Layer::onEnter();
    
    //タッチイベントの設定
    initEventLitsener();
    
    //画像の配置
    createImage();
    
    //アニメーションの開始
    scheduleOnce(schedule_selector(TournamentStartAnimationLayer::animationStart), 1.0f);
}

void TournamentStartAnimationLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TournamentStartAnimationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentStartAnimationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentStartAnimationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentStartAnimationLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentStartAnimationLayer::createImage()
{
    //背景の影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    //背景の帯
    auto bk_obi = Sprite::create("init/Home/Event/TournamentEvent/bk_event_start.png");
    bk_obi->setScale(ratio);
    bk_obi->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bk_obi, Z_Obi, T_Obi);
    
    //貂蝉の画像
    auto navi = Sprite::create("init/Dungeon/result/navi.png");
    navi->setScale(ratio);
    navi->setPosition(convertFromDisplaySize(Point(DEFAULT_POS - (_contentSize.width), 480), 0.5, 0.5));
    this->addChild(navi, Z_Navi, T_Navi);
    
    //メッセージの作成
    string navi_text = "トーナメント\n対戦時間になりました!";
    auto navi_label = HMSFontUtil::createWithTTFBySystemFontArgument(navi_text, FONT_NAME_SYSTEM, 40);
    navi_label->setPosition(Point(navi->getContentSize().width / 2 + 340, navi->getContentSize().height / 2));
    navi->addChild(navi_label, Z_Message, T_Message);
}

void TournamentStartAnimationLayer::animationStart(float dt )
{
    //アニメーションした後に消す
    auto navi = (Sprite *)this->getChildByTag(T_Navi);
    auto bk_obi = (Sprite *)this->getChildByTag(T_Obi);
    
    auto move1 = MoveTo::create(0.5f, convertFromDisplaySize(Point(DEFAULT_POS, 480), 0.5, 0.5));
    auto ease1 = EaseOut::create(move1, 3);
    auto move2 = MoveTo::create(1.5f, convertFromDisplaySize(Point(DEFAULT_POS + 14, 480), 0.5, 0.5));
    auto move3 = MoveTo::create(0.5f, convertFromDisplaySize(Point(DEFAULT_POS + 300 + (_contentSize.width), 480), 0.5, 0.5));
    auto ease2 = EaseIn::create(move3, 3);
    
    auto scale = ScaleTo::create(0.2f, 1.0, 0);
    
    auto func = CallFunc::create([this](){
        
        auto normal_scene = (NormalScene *)_node;
        normal_scene->changePage("TournamentEventBattleWait");
    });
    
    runAction(Sequence::create(TargetedAction::create(navi, Sequence::create(ease1, move2, ease2, nullptr)),
                               TargetedAction::create(bk_obi, Sequence::create(scale, DelayTime::create(1.0f), nullptr)),
                               func,
                               TargetedAction::create(this, RemoveSelf::create(true)),
                               nullptr));
}

bool TournamentStartAnimationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch TournamentStartAnimationLayer!");
    
    return true;
}
void TournamentStartAnimationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentStartAnimationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentStartAnimationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}