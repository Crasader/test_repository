#include "CharaCollectionGetRewardPopup.h"
#include "Utility.h"

CharaCollectionGetRewardPopup* CharaCollectionGetRewardPopup::create(string rewardItemType, string rewardItemId, string comment, string count, string charaName)
{
    CharaCollectionGetRewardPopup *node = new CharaCollectionGetRewardPopup();
    if (node && node->init(rewardItemType, rewardItemId, comment, count, charaName))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool CharaCollectionGetRewardPopup::init(string rewardItemType, string rewardItemId, string comment, string count, string charaName)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaCollectionGetRewardPopup::init()");
    
    ratio = getVisibleRatio();
    
    this->rewardItemType    = rewardItemType ;
    this->rewardItemId      = rewardItemId ;
    this->comment           = comment;
    this->itemCount         = count;
    this->charaName         = charaName;
    this->actionFlg         = false;
    
    return true;
}
void CharaCollectionGetRewardPopup::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //スプライトの生成
    createSprite() ;
    
    //runActionの作成
    createRunAction() ;
}

void CharaCollectionGetRewardPopup::createRunAction()
{
    //アクションフラグ
    actionFlg = true;

        
    CallFunc* sound = CallFunc::create([](){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/get_item.mp3");
    });
        
    //ライトの回転
    RotateBy* rotate = RotateBy::create(10.0f, 360);
    Repeat* repeat = Repeat::create(rotate, -1);
    light->runAction(repeat);
    
    //報酬画像の移動、移動後ライト表示
    MoveTo* moveReward = MoveTo::create(0.6f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    EaseElasticOut* easeReward = EaseElasticOut::create(moveReward, 1.5f);
    CallFunc* funcLight = CallFunc::create([this](){
        light->setVisible(true);
        
        //アクションフラグ変更（ここからタッチ可能にする）
        actionFlg = false;
    });
    booty->runAction(Sequence::create(DelayTime::create(1.0f), easeReward, funcLight, nullptr));
    
    //ナビキャラの表示
    MoveTo* moveChara = MoveTo::create(0.2f, convertFromDisplaySize(Point(0, 0) ,0.5 ,0));
    EaseOut* easeChara = EaseOut::create(moveChara, 2);
    chara->runAction(Sequence::create(DelayTime::create(1.5f), easeChara, nullptr));
    
    //任務達成文字の表示
    ScaleTo* scaleTitle = ScaleTo::create(0.5f, 1.0f * ratio);
    EaseElasticOut* easeTitle  = EaseElasticOut::create(scaleTitle, 1);
    rewardTitle->runAction(Sequence::create(DelayTime::create(1.6f), easeTitle, nullptr));
        
    //ナビキャラ台詞の吹き出しと文字の表示
    ScaleTo* scaleBalloon = ScaleTo::create(0.5f, 1.0f * ratio);
    EaseSineOut* easeBalloon  = EaseSineOut::create(scaleBalloon);
    CallFunc* funcBootyLabel = CallFunc::create([this](){
        bootyLabel->setVisible(true);
    });
    naviBalloon->runAction(Sequence::create(DelayTime::create(0.5f), sound, DelayTime::create(1.2f), easeBalloon, funcBootyLabel, nullptr));
    
}

void CharaCollectionGetRewardPopup::createSprite()
{
    //背景画像の作成
    createBg();
    
    //報酬画像の生成
    createBooty() ;
    
    //報酬文字の生成
    createBootyTitle();
    
    //キャラクターの生成
    createChara() ;
    
    //上側の「〜達成」のスプライトを生成
    createRewardTitle() ;
    
    //後ろの光の生成
    createLight() ;
    
    //紙吹雪の作成
    createConfetti() ;
    
    //吹き出しを作成
    createNaviBaloon();
    
}

void CharaCollectionGetRewardPopup::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CharaCollectionGetRewardPopup::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaCollectionGetRewardPopup::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaCollectionGetRewardPopup::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaCollectionGetRewardPopup::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void CharaCollectionGetRewardPopup::createNaviBaloon()
{
    //吹き出しの作成
    naviBalloon = Sprite::create("init/Home/Beginner/Clear/navi_balloon.png") ;
    naviBalloon->setScale(0) ;
    naviBalloon->setAnchorPoint(Point(0, 0.5)) ;
    naviBalloon->setPosition(convertFromDisplaySize(Point(chara->getContentSize().width * 2 / 3, chara->getContentSize().height/2), 0.5, 0)) ;//　左下らへん
    this->addChild(naviBalloon, O_balloon) ;
    
    string balloonComment = Utility::replaceStr(comment, "n", "\n");
    auto balloonLabel = HMSFontUtil::createWithTTFBySystemFontArgument(balloonComment , FONT_NAME_SYSTEM, 24);
    balloonLabel->setAlignment(TextHAlignment::LEFT);
    balloonLabel->setDimensions(290 * ratio, 160 * ratio);
    balloonLabel->setAnchorPoint(Point(0, 1.0));
    balloonLabel->setTextColor(Color4B(20, 20, 20, 255));
    balloonLabel->enableOutline(Color4B(255, 255, 255, 255), 1);
    balloonLabel->setPosition(Point(76, 166)) ;
    naviBalloon->addChild(balloonLabel, O_balloon_label);
    
}
void CharaCollectionGetRewardPopup::createConfetti()
{
    //紙吹雪の生成
    confetti01 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
    confetti01->setAnchorPoint(Point(0.5, 0));
    confetti01->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0.0)) ;
    confetti01->setScale(ratio) ;
    this->addChild(confetti01, O_confetti) ;
    
    
    //紙吹雪の生成
    confetti02 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
    confetti02->setAnchorPoint(Point(0.5, 0));
    confetti02->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, confetti02->getContentSize().height), 0.5, 0.0)) ;
    confetti02->setScale(ratio) ;
    this->addChild(confetti02, O_confetti) ;
    
    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    confetti01->setBlendFunc(blend);
    confetti02->setBlendFunc(blend);
        
    schedule(schedule_selector(CharaCollectionGetRewardPopup::moveConfetti), 0.01f);
    
}

void CharaCollectionGetRewardPopup::moveConfetti(float dt)
{
    confetti01->setPosition(confetti01->getPosition().x, confetti01->getPosition().y - 1.5f*ratio);
    confetti02->setPosition(confetti02->getPosition().x, confetti02->getPosition().y - 1.5f*ratio);
    
    if (confetti01->getPosition().y <= -confetti01->getContentSize().height*ratio)
    {
        confetti01->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
    
    if (confetti02->getPosition().y <= -confetti02->getContentSize().height*ratio)
    {
        confetti02->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
}

void CharaCollectionGetRewardPopup::createLight()
{
    //ライトの生成
    light = Sprite::create("init/Home/Beginner/Clear/light.png") ;
    light->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5)) ;
    light->setScale(ratio) ;
    light->setVisible(false);
    
    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    light->setBlendFunc(blend);
    
    this->addChild(light, O_light) ;
}

void CharaCollectionGetRewardPopup::createRewardTitle()
{
    rewardTitle = Sprite::create("init/CharaMenu/book/title_achieve.png") ;
    rewardTitle->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT*4/5) ,0.5 ,0.5)) ; //画面のちょっと上の位置
    rewardTitle->setScale(0); //最初は小さく
    //    rewardTitle->setScale(ratio) ;
    this->addChild(rewardTitle, O_reward_title) ;
}

void CharaCollectionGetRewardPopup::createChara()
{

    chara = Sprite::create("init/CharaMenu/book/navi1.png") ;
    chara->setScale(ratio) ;
    chara->setAnchorPoint(Point(0,0)) ;
    chara->setPosition(convertFromDisplaySize(Point(-chara->getContentSize().width*ratio, -chara->getContentSize().height*ratio/3) ,0 ,0)) ; //左下の見えない部分
    this->addChild(chara, O_chara) ;
}

void CharaCollectionGetRewardPopup::createBooty()
{
    if (rewardItemType == "chara"){
        //カード画像のパスを取得
        string card_id = rewardItemId ;
        string card_pass = "download/Chara/all400/" + card_id + ".png";
        
        booty = Sprite::create(card_pass);
        
    }else if (rewardItemType == "gold"){
        booty = Sprite::create("init/Home/Beginner/Clear/reward_l_1.png");
        booty->setScale(1);
        
    }else if (rewardItemType == "jewel"){
        booty = Sprite::create("init/Home/Beginner/Clear/reward_l_2.png");
        booty->setScale(1) ;
    }else if(rewardItemType == "exchange_ticket"){
        booty = Sprite::create("init/Home/DailyMission/Clear/reward_l_3.png");
        booty->setScale(1) ;
    }
    
    booty->setPosition(Point(_contentSize.width / 2, _contentSize.height + booty->getContentSize().height)) ;
    booty->setScale(ratio) ;
    this->addChild(booty, O_booty) ;
}

void CharaCollectionGetRewardPopup::createBootyTitle()
{
    //報酬文字
    string booty_title = "";
    if (rewardItemType == "chara"){
        booty_title = charaName;
    }else if (rewardItemType == "gold"){
        booty_title = "資金";
    }else if (rewardItemType == "jewel"){
        booty_title = "宝玉";
    }else if (rewardItemType == "exchange_ticket"){
        booty_title = "交換チケット";
    }
    
    string text = booty_title + "×" + itemCount + "を手に入れました";
    bootyLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text , FONT_NAME_SYSTEM, 35);
    bootyLabel->setAlignment(TextHAlignment::CENTER);
    bootyLabel->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - booty->getContentSize().height / 2), 0.5, 0.5)) ;
    bootyLabel->enableOutline(Color4B::BLACK, 4);
    bootyLabel->setScale(ratio);
    bootyLabel->setVisible(false);
    this->addChild(bootyLabel, O_booty_label);
}

void CharaCollectionGetRewardPopup::createBg()
{
    //影
    Sprite* shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, O_shadow);
}

void CharaCollectionGetRewardPopup::deleteAction()
{
    Vector<Node*> children = this->getChildren();
    
    for (Node* child : children)
    {
        if (child->getTag() == T_Popup)
        {
            ScaleTo* scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        //閉じられたときに呼び出しもとレイヤーが登録したコールバックを呼び出してあげる
        if(parent!=nullptr && selForClose!=nullptr) {
            parent->scheduleOnce(selForClose, 0.0f);
            selForClose = nullptr;
        }
        
        this->removeFromParent();
    });
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void CharaCollectionGetRewardPopup::setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel){
    this->parent = parent;
    this->selForClose = sel;
}

bool CharaCollectionGetRewardPopup::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch CharaCollectionGetRewardPopup!");
    
    if (actionFlg != true) {
        deleteAction();
    }
    
    return true;
}
void CharaCollectionGetRewardPopup::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaCollectionGetRewardPopup::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaCollectionGetRewardPopup::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
