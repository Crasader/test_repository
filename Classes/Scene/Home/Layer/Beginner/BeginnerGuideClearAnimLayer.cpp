#include "BeginnerGuideClearAnimLayer.h"

BeginnerGuideClearAnimLayer* BeginnerGuideClearAnimLayer::create(string reward_item_type, string reward_related_id, string comment, string count, string chara_name, bool all_complate_anim_flg)
{
    BeginnerGuideClearAnimLayer *node = new BeginnerGuideClearAnimLayer();
    if (node && node->init(reward_item_type, reward_related_id, comment, count, chara_name, all_complate_anim_flg))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool BeginnerGuideClearAnimLayer::init(string reward_item_type, string reward_related_id, string comment, string count, string chara_name, bool all_complate_anim_flg)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BeginnerGuideClearAnimLayer::init()");
    
    ratio = getVisibleRatio();
    
    _reward_item_type  = reward_item_type ;
    _reward_related_id = reward_related_id ;
    _comment           = comment;
    _count             = count;
    _chara_name        = chara_name;
    _all_complate_anim_flg = all_complate_anim_flg ;
    _action_flg = false;
    
    return true;
}
void BeginnerGuideClearAnimLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //スプライトの生成
    createSprite() ;
    
    //runActionの作成
    createRunAction() ;
}

void BeginnerGuideClearAnimLayer::createRunAction()
{
    //アクションフラグ
    _action_flg = true;
    
    if(_all_complate_anim_flg){
        
        auto sound = CallFunc::create([](){
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/all_complete.mp3");
        });
        
        //ライトの回転
        auto rotate = RotateBy::create(10.0f, 360);
        auto repeat = Repeat::create(rotate, -1);
        _light->runAction(repeat);
        
        //報酬画像の移動、移動後ライト表示
        auto move_reward = MoveTo::create(0.6f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
        //        auto ease_reward = EaseBounceOut::create(move_reward);
        auto ease_reward = EaseElasticOut::create(move_reward, 1.0f);
        auto func_light = CallFunc::create([this](){
            _light->setVisible(true);
            
            //アクションフラグ変更（ここからタッチ可能にする）
            _action_flg = false;
        });
        _booty->runAction(Sequence::create(DelayTime::create(1.0f), ease_reward, func_light, nullptr));
        
        //ナビキャラの表示
        auto move_chara = MoveTo::create(0.2f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0) ,0.5 ,0));
        auto ease_chara = EaseOut::create(move_chara, 2);
        
        auto move_chara2  = MoveBy::create(2.0f, Point(0, -20));
        auto repeat_chara = Repeat::create(Sequence::create(move_chara2, move_chara2->reverse(), nullptr), -1);
        _chara->runAction(Sequence::create(DelayTime::create(1.5f), ease_chara, repeat_chara, nullptr));
        
        //任務達成文字の表示
        auto scale_title = ScaleTo::create(0.5f, 1.0f * ratio);
        auto ease_title  = EaseElasticOut::create(scale_title, 1);
        auto func_booty_label = CallFunc::create([this](){
            _booty_label->setVisible(true);
        });
        _reward_title->runAction(Sequence::create(sound, DelayTime::create(1.6f), ease_title, func_booty_label, nullptr));
    }
    else{
        
        auto sound = CallFunc::create([](){
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/get_item.mp3");
        });
        
        //ライトの回転
        auto rotate = RotateBy::create(10.0f, 360);
        auto repeat = Repeat::create(rotate, -1);
        _light->runAction(repeat);
        
        //報酬画像の移動、移動後ライト表示
        auto move_reward = MoveTo::create(0.6f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
//        auto ease_reward = EaseBounceOut::create(move_reward);
        auto ease_reward = EaseElasticOut::create(move_reward, 1.5f);
        auto func_light = CallFunc::create([this](){
            _light->setVisible(true);
            
            //アクションフラグ変更（ここからタッチ可能にする）
            _action_flg = false;
        });
        _booty->runAction(Sequence::create(DelayTime::create(1.0f), ease_reward, func_light, nullptr));
        
        //ナビキャラの表示
        auto move_chara = MoveTo::create(0.2f, convertFromDisplaySize(Point(0, 0) ,0.5 ,0));
        auto ease_chara = EaseOut::create(move_chara, 2);
        _chara->runAction(Sequence::create(DelayTime::create(1.5f), ease_chara, nullptr));
        
        //任務達成文字の表示
        auto scale_title = ScaleTo::create(0.5f, 1.0f * ratio);
        auto ease_title  = EaseElasticOut::create(scale_title, 1);
        _reward_title->runAction(Sequence::create(DelayTime::create(1.6f), ease_title, nullptr));
        
        //ナビキャラ台詞の吹き出しと文字の表示
        auto scale_balloon = ScaleTo::create(0.5f, 1.0f * ratio);
        auto ease_balloon  = EaseSineOut::create(scale_balloon);
        auto func_booty_label = CallFunc::create([this](){
            _booty_label->setVisible(true);
        });
        _navi_balloon->runAction(Sequence::create(DelayTime::create(0.5f), sound, DelayTime::create(1.2f), ease_balloon, func_booty_label, nullptr));
    }
}

void BeginnerGuideClearAnimLayer::createSprite()
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

void BeginnerGuideClearAnimLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BeginnerGuideClearAnimLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BeginnerGuideClearAnimLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BeginnerGuideClearAnimLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BeginnerGuideClearAnimLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void BeginnerGuideClearAnimLayer::createNaviBaloon()
{
    if(_all_complate_anim_flg){
    }else{
        //吹き出しの作成
        _navi_balloon = Sprite::create("init/Home/Beginner/Clear/navi_balloon.png") ;
        _navi_balloon->setScale(0) ;
        _navi_balloon->setAnchorPoint(Point(0, 0.5)) ;
        _navi_balloon->setPosition(convertFromDisplaySize(Point(_chara->getContentSize().width * 2 / 3, _chara->getContentSize().height/2), 0.5, 0)) ;//　左下らへん
        this->addChild(_navi_balloon, O_balloon) ;
        
        string balloon_comment = Utility::replaceStr(_comment, "n", "\n");
        auto balloon_label = HMSFontUtil::createWithTTFBySystemFontArgument(balloon_comment , FONT_NAME_SYSTEM, 32);
        balloon_label->setAlignment(TextHAlignment::LEFT);
        balloon_label->setDimensions(290 * ratio, 160 * ratio);
        balloon_label->setAnchorPoint(Point(0, 1.0));
        balloon_label->setTextColor(Color4B(20, 20, 20, 255));
        balloon_label->enableOutline(Color4B(255, 255, 255, 255), 1);
        balloon_label->setPosition(Point(76, 166)) ;
        _navi_balloon->addChild(balloon_label, O_balloon_label);
    }
}
void BeginnerGuideClearAnimLayer::createConfetti()
{
    if(_all_complate_anim_flg){
        //紙吹雪の生成
        _confetti_1 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
        _confetti_1->setAnchorPoint(Point(0.5, 0));
        _confetti_1->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0.0)) ;
//        _confetti_1->setAnchorPoint(Point(0.5, 0.5));
//        _confetti_1->setPosition(Point(_confetti_1->getContentSize() / 2));
        _confetti_1->setScale(ratio) ;
        this->addChild(_confetti_1, O_confetti) ;
        
        
        //紙吹雪の生成
        _confetti_2 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
        _confetti_2->setAnchorPoint(Point(0.5, 0));
        _confetti_2->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, _confetti_2->getContentSize().height), 0.5, 0.0)) ;
//        _confetti_2->setAnchorPoint(Point(0.5, 0.5));
//        _confetti_2->setPosition(Point(_confetti_2->getContentSize() / 2));
        _confetti_2->setScale(ratio) ;
        this->addChild(_confetti_2, O_confetti) ;
        
        //ブレンド　（加算合成）
        BlendFunc blend;
        blend.src = GL_SRC_ALPHA;
        blend.dst = GL_ONE;
        _confetti_1->setBlendFunc(blend);
        _confetti_2->setBlendFunc(blend);
        
        schedule(schedule_selector(BeginnerGuideClearAnimLayer::moveConfetti), 0.01f);
    }
}

void BeginnerGuideClearAnimLayer::moveConfetti(float dt)
{
    _confetti_1->setPosition(_confetti_1->getPosition().x, _confetti_1->getPosition().y - 1.5f*ratio);
    _confetti_2->setPosition(_confetti_2->getPosition().x, _confetti_2->getPosition().y - 1.5f*ratio);
    
    if (_confetti_1->getPosition().y <= -_confetti_1->getContentSize().height*ratio)
    {
        _confetti_1->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
    
    if (_confetti_2->getPosition().y <= -_confetti_2->getContentSize().height*ratio)
    {
        _confetti_2->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
}

void BeginnerGuideClearAnimLayer::createLight()
{
    //ライトの生成
    _light = Sprite::create("init/Home/Beginner/Clear/light.png") ;
    _light->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5)) ;
    _light->setScale(ratio) ;
    _light->setVisible(false);
    
    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    _light->setBlendFunc(blend);
    
    this->addChild(_light, O_light) ;
}

void BeginnerGuideClearAnimLayer::createRewardTitle()
{
    if(_all_complate_anim_flg){
        _reward_title = Sprite::create("init/Home/Beginner/Clear/title_comp.png") ;
        _reward_title->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT*5/6) ,0.5 ,0.5)) ; //画面のちょっと上の位置
        
    }else{
        _reward_title = Sprite::create("init/Home/Beginner/Clear/title_achieve.png") ;
        _reward_title->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT*4/5) ,0.5 ,0.5)) ; //画面のちょっと上の位置
    }
    
    _reward_title->setScale(0); //最初は小さく
//    _reward_title->setScale(ratio) ;
    this->addChild(_reward_title, O_reward_title) ;
}

void BeginnerGuideClearAnimLayer::createChara()
{
    if(_all_complate_anim_flg){
        _chara = Sprite::create("init/Home/Beginner/Clear/navi2.png") ;
        _chara->setScale(ratio) ;
        _chara->setAnchorPoint(Point(0.5,0)) ;
        _chara->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, -_chara->getContentSize().height*ratio) ,0.5 ,0)) ; //下側の見えない部分
//        _chara->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0) ,0.5 ,0)) ; //下にぴったり
    }else{
        _chara = Sprite::create("init/Home/Beginner/Clear/navi1.png") ;
        _chara->setScale(ratio) ;
        _chara->setAnchorPoint(Point(0,0)) ;
        _chara->setPosition(convertFromDisplaySize(Point(-_chara->getContentSize().width*ratio, -_chara->getContentSize().height*ratio/3) ,0 ,0)) ; //左下の見えない部分
//        _chara->setPosition(convertFromDisplaySize(Point(0, 0) ,0.5 ,0)) ; //左下から見える
    }
    this->addChild(_chara, O_chara) ;
}

void BeginnerGuideClearAnimLayer::createBooty()
{
    if (_reward_item_type == "chara")
    {
        //カード画像のパスを取得
        string card_id = _reward_related_id ;
        string card_pass = "download/Chara/all400/" + card_id + ".png";
        
        _booty = Sprite::create(card_pass) ;
    }
    else if (_reward_item_type == "gold")
    {
        _booty = Sprite::create("init/Home/Beginner/Clear/reward_l_1.png");
        _booty->setScale(1) ;
    }
    else if (_reward_item_type == "jewel")
    {
        _booty = Sprite::create("init/Home/Beginner/Clear/reward_l_2.png");
        _booty->setScale(1) ;
    }
    
    _booty->setPosition(Point(_contentSize.width / 2, _contentSize.height + _booty->getContentSize().height)) ;
    _booty->setScale(ratio) ;
    this->addChild(_booty, O_booty) ;
}

void BeginnerGuideClearAnimLayer::createBootyTitle()
{
    //報酬文字
    string booty_title = "";
    if (_reward_item_type == "chara")
    {
        booty_title = _chara_name;
    }
    else if (_reward_item_type == "gold")
    {
        booty_title = "資金";
    }
    else if (_reward_item_type == "jewel")
    {
        booty_title = "宝玉";
    }
    
    string text = booty_title + "×" + _count + "を手に入れました";
    _booty_label = HMSFontUtil::createWithTTFBySystemFontArgument(text , FONT_NAME_SYSTEM, 35);
    _booty_label->setAlignment(TextHAlignment::CENTER);
    _booty_label->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - _booty->getContentSize().height / 2), 0.5, 0.5)) ;
    _booty_label->enableOutline(Color4B::BLACK, 4);
    _booty_label->setScale(ratio);
    _booty_label->setVisible(false);
    this->addChild(_booty_label, O_booty_label);
}

void BeginnerGuideClearAnimLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, O_shadow);
}

/*
void BeginnerGuideClearAnimLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
}
 */

void BeginnerGuideClearAnimLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == T_Popup)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        //閉じられたときに呼び出しもとレイヤーが登録したコールバックを呼び出してあげる
        if(_parent!=nullptr && _sel_for_close!=nullptr) {
            _parent->scheduleOnce(_sel_for_close, 0.0f);
            _sel_for_close = nullptr;
        }
        
        this->removeFromParent();
    });
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void BeginnerGuideClearAnimLayer::setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel){
    _parent = parent;
    _sel_for_close = sel;
}

bool BeginnerGuideClearAnimLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch BeginnerGuideClearAnimLayer!");
    
    if (_action_flg != true) {
        deleteAction();
    }
    
    return true;
}
void BeginnerGuideClearAnimLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BeginnerGuideClearAnimLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BeginnerGuideClearAnimLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
