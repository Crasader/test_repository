#include "GachaResultAnimationLayer.h"

bool GachaResultAnimationLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("GachaResultAnimationLayer::init()");
    initEventListener();
    
    _now_card_index = 0;
    _now_animation = false ;
    _touch_enable = true;
    _animation_end = false ;
	
	info_flg = false;
    
    return true;
}

void GachaResultAnimationLayer::setBoxGachaFlg(bool box_gacha_flg)
{
    _box_gacha_flg = box_gacha_flg;
}

void GachaResultAnimationLayer::setData(int gacha_type,int card_count,int hit_card[],int rare_card[],int chara_type[], int chara_list_id[]){
    
    setGachaType(gacha_type);
    setCardCount(card_count);
    for(int i=0;i<_card_count;i++){
        _hit_card[i]=hit_card[i];
        _rare_card[i]=rare_card[i];
        _chara_type[i]=chara_type[i];
        _chara_list_id[i]=chara_list_id[i];
    }
}

void GachaResultAnimationLayer::getSize(){
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    main_screen_size_split_10 = main_screen_size / 10;
    main_screen_size_split_5 = main_screen_size / 5;
}

void GachaResultAnimationLayer::createBg(){
    auto bg = Sprite::create("init/Home/Gacha/back2.png");
    bg->setScale(1.0 * ratio,1.2f * ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, visibleSize.height / 2 /ratio), 0.5, 0.5));
    this->addChild(bg, 1);
}

void GachaResultAnimationLayer::setCardByIndex(int index){
    //裏のカード
    _card_back = Sprite::create("download/Chara/parts100/card_bk1_reverse.png");
    _card_back->setVisible(false) ;
    _card_back->setAnchorPoint(Point(0.5, 0.5)) ;
    _card_back->setScale(4.0 * ratio);//1.0 = 100x100
    _card_back->setPosition(convertFromDisplaySize(Point((DISPLAY_WIDTH/ 2), DISPLAY_HEIGHT/2), 0.5, 0.5));

    //表カード
    string card_format = cocos2d::StringUtils::format("download/Chara/all400/%d.png", _hit_card[index]);
    _card_open= Sprite::create(card_format);
    _card_open->setVisible(false) ;
    _card_open->setAnchorPoint(Point(0.5, 0)) ;
    _card_open->setScale(ratio);//1.0 = 400x400
    _card_open->setPosition(convertFromDisplaySize(Point((DISPLAY_WIDTH / 2), DISPLAY_HEIGHT/2-_card_open->getContentSize().height/2), 0.5, 0.5));
    
    Sprite *icon;
    if(_chara_type[index]==1){
        icon = Sprite::create("init/CharaMenu/icon_offense2.png");
    }else
    if(_chara_type[index]==2){
        icon = Sprite::create("init/CharaMenu/icon_defense2.png");
    }else
    if(_chara_type[index]==3){
        icon = Sprite::create("init/CharaMenu/icon_strategist2.png");
    }else
    if(_chara_type[index]==4){
        icon = Sprite::create("init/CharaMenu/icon_general2.png");
    }
    icon->setScale(2.0f);
    icon->setPosition(convertFromDisplaySize(Point(55/ratio, 50/ratio), 0, 0));
    _card_open->addChild(icon, 1);
    HMSLOG("setcard!!!!=%index",_hit_card[index]);
    
    this->addChild(_card_back,5+index);
    this->addChild(_card_open,5+index);
}

void GachaResultAnimationLayer::setGachaInfo(){
	if(!info_flg) {
		_card_index_bg = Sprite::create("init/Title/title_version_back.png");
		_card_index_bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH-(_card_index_bg->getContentSize().width/2)-10, 10), 0.5, 0.0));
		_card_index_bg->setAnchorPoint(Point(0.5, 0));
		_card_index_bg->setScale(ratio);
		this->addChild(_card_index_bg, 10);
		
		string gacha_info_text =  to_string(_now_card_index + 1) + "/" + to_string(_card_count);
		_info_label = HMSFontUtil::createWithTTFBySystemFontArgument(gacha_info_text,FONT_NAME_SYSTEM,34);
		_info_label->setAlignment(TextHAlignment::CENTER);
		_info_label->setTextColor(Color4B::WHITE);
		_info_label->setPosition(_card_index_bg->getContentSize()/2);
		_info_label->enableOutline(Color4B::BLACK, 2);
		_card_index_bg->addChild(_info_label);
		info_flg = true;
		
		//スキップボタン
		Sprite* skip_img = Sprite::create("init/Title/title_version_back.png");
		Label* skip_l = HMSFontUtil::createWithTTFBySystemFontArgument("スキップ",FONT_NAME_SYSTEM,34);
		skip_l->setAlignment(TextHAlignment::CENTER);
		skip_l->setTextColor(Color4B::WHITE);
		skip_l->setPosition(skip_img->getContentSize()/2);
		skip_l->enableOutline(Color4B::BLACK, 2);
		skip_img->addChild(skip_l);
		MenuItemSprite* skip_b = MenuItemSprite::create(skip_img, skip_img, [&](Ref* pSender){
			animationEnd();
		});
		skip_b->setPosition(Point(convertFromDisplaySize(Point(10, 10), 0.5, 0.0)));
		skip_b->setAnchorPoint(Point::ZERO);
		skip_b->setScale(ratio);
		
		Menu* skip_m = Menu::create(skip_b, NULL);
		skip_m->setPosition(convertFromDisplaySize(Point::ZERO, 0.5, 0.0));
		this->addChild(skip_m, 10);
	} else {
		string gacha_info_text =  to_string(_now_card_index + 1) + "/" + to_string(_card_count);
		_info_label->setString(gacha_info_text);
	}
}

void GachaResultAnimationLayer::updateForResultFlow(float dt){
    //スケジュールを削除
    this->unschedule(schedule_selector(GachaResultAnimationLayer::updateForResultFlow));
    se_bool=true;
    
    if(_card_count == _now_card_index){
        animationEnd() ;
        return ;
    }
    
    if(_now_card_index != 0){
        this->stopAllActions() ;
        
        _card_back->stopAllActions() ;
        _card_open->stopAllActions() ;
        _chara_detail_sprite->stopAllActions() ;
        _chara_detail_sprite->removeFromParentAndCleanup(true) ;
        _card_back->removeFromParentAndCleanup(true) ;
        _card_open->removeFromParentAndCleanup(true) ;
        
        for(Node *node : getChildren()){
            if(node->getTag() == TAG_RERITY || node->getTag() == TAG_EFFECT){
                node->stopAllActions() ;
                node->setVisible(false) ;
            }
        }
        for(Node *node : getChildren()){
            if(node->getTag() == TAG_RERITY || node->getTag() == TAG_EFFECT){
                node->removeFromParentAndCleanup(true) ;
            }
        }
    }
    
    _now_animation = true;
    
    //カードの設置
    setCardByIndex(_now_card_index);
	
	if(_card_count > 1) setGachaInfo();
    
    //カードの詳細スプライトの作成
    setCharaDetailByIndex(_now_card_index);
    
    resultActionByCardIndex(_now_card_index) ;
    
    _now_card_index++ ;
}

void GachaResultAnimationLayer::resultActionForceEnd(int card_index){
    //全てのアクションを全て強制終了する
    _card_back->stopAllActions() ;
    _card_open->stopAllActions() ;
    auto default_orbit_camera = OrbitCamera::create(0, 1, 0, 0, 0, 0, 0);//カメラが軌道の途中で止まっている可能性があるので初期位置に0秒で戻す
    _card_open->runAction(default_orbit_camera) ;
    _chara_detail_sprite->stopAllActions() ;
    deleteRarityAction() ;
    
    //ガチャアニメーション短縮エフェクト音(１回)
    if(se_bool) CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/slash3.mp3",false);
    se_bool=false;
    
    //アクションが終了した時点で表示される内容をここで形成する
    _card_back->setVisible(false) ;
    
    _card_open->setVisible(true) ;
    _card_open->setScale(ratio*5.0/6.0) ;
    _card_open->setPosition(convertFromDisplaySize(Point((DISPLAY_WIDTH / 2),DISPLAY_HEIGHT/2+50), 0.5, 0.5));
    
    _chara_detail_sprite->setScale(ratio) ;
    _now_animation = false ;
}

void GachaResultAnimationLayer::resultActionByCardIndex(int card_index){
    
    float dark_end_time = 0.2f ;
    if(card_index!=0){
        _black_sprite->setOpacity(0) ;
        _black_sprite->runAction(Sequence::create(Show::create(), FadeIn::create(dark_end_time), FadeOut::create(0.2f) , Hide::create(), nullptr)) ;
    }
    
    float rotation_90_time = 0.3 ;
    
    //カード裏面が90度回転するアクション
    auto card_close = OrbitCamera::create(rotation_90_time, 1, 0, 0, 90, 0, 0);
    auto card_close_ease = EaseExponentialIn::create(card_close);
    
    //カード表面が90度回転するアクション
    auto card_open = OrbitCamera::create(rotation_90_time, 1, 0, 270, 90, 0, 0);
    auto card_open_ease = EaseExponentialOut::create(card_open);

    //レアリティが出ながら光るエフェクト
    CallFunc *effect_func = CallFunc::create([this,card_index]() {
                                        effect() ;
                                        popupRarity(card_index) ;
    });
   
    //カードが大きくなるアクション(あらかじめ0.5倍のスケールにしておく)
    _card_back->setScale(ratio*2.0f);//1.0 = 400x400
    
    float zoom_time = 0.3f ;
    auto card_zoom_ease = EaseExponentialIn::create(ScaleTo::create(zoom_time, ratio*4.0));
    
    //回転後の空白時間
    float after_rotation_blank = 0.5f;
    
    //カードが上に移動しつつ縮小
    float detail_show_action_time = 0.3f;
    auto detail_show_action_for_card_open = EaseExponentialOut::create(Spawn::createWithTwoActions(ScaleTo::create(detail_show_action_time, ratio*5.0/6.0), MoveTo::create(detail_show_action_time, convertFromDisplaySize(Point((DISPLAY_WIDTH / 2), DISPLAY_HEIGHT/2+50), 0.5, 0.5)))) ;
    
    auto detail_show_action_for_chara_detail = EaseExponentialOut::create(ScaleTo::create(detail_show_action_time, ratio));
    
    //最後まで表示し終わっていたらnow_animationをfalseにする
    CallFunc *now_anim_false_func= CallFunc::create([this]() {
        _now_animation = false ;
    });
    
    //カード裏面画像のアクション ズームしてから引っくり返る
    _card_back->runAction(Sequence::create(DelayTime::create(dark_end_time), Show::create(), card_zoom_ease, card_close_ease, Hide::create(), nullptr));
    //カード画像のアクション　エフェクトを出してから引っくり返る
    _card_open->runAction(Sequence::create(DelayTime::create(dark_end_time), DelayTime::create(zoom_time), DelayTime::create(rotation_90_time), Show::create(), effect_func, card_open_ease,DelayTime::create(after_rotation_blank),detail_show_action_for_card_open, nullptr));
    //カードのアクションがあった後詳細を表示する
    _chara_detail_sprite->runAction(Sequence::create(DelayTime::create(dark_end_time), DelayTime::create(zoom_time), DelayTime::create(rotation_90_time*2.0),DelayTime::create(after_rotation_blank),CallFunc::create(CC_CALLBACK_0(GachaResultAnimationLayer::deleteRarityAction,this)), detail_show_action_for_chara_detail, now_anim_false_func, nullptr)) ;
    
}

void GachaResultAnimationLayer::effect(){
    //ガチャアニメーションエフェクト音(１回)
    if(se_bool) CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/slash3.mp3",false);
    se_bool=false;
    
    ParticleSystemQuad *particle = ParticleSystemQuad::create("download/Effect/Particle/twinkle.plist");
    particle->setScale(1.0*ratio);
    particle->setAutoRemoveOnFinish(true);
    particle->setPosition(convertFromDisplaySize(Point(320,480), 0.5, 0.5));
    this->addChild(particle, 300, TAG_EFFECT);
    particle->runAction(Sequence::create(DelayTime::create(1.0), RemoveSelf::create(), nullptr)) ;
}

void GachaResultAnimationLayer::deleteRarityAction(){
        for(Node *node : getChildren()){
            if(node->getTag() == TAG_RERITY || node->getTag() == TAG_EFFECT){
                node->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f,0)));
            }
        }
}

void GachaResultAnimationLayer::popupRarity(int card_index){
    int rarity = _rare_card[card_index];//カードのレアリティ
    int x_point[3];
    
    if(rarity>=4){
        x_point[2]=main_screen_w / 2 - 180;
        x_point[1]=main_screen_w / 2 - 60;
        x_point[0]=main_screen_w / 2 + 60;
    }
    else if(rarity==3){
        x_point[1]=main_screen_w / 2 - 120;
        x_point[0]=main_screen_w / 2;
    }
    else if(rarity==2){
        x_point[0]=main_screen_w / 2 - 60;
    }
    if(rarity>=4){
        auto rarity_1 = Sprite::create("download/Gacha/rare3_150x150.png");//文字「超」
        rarity_1->setScale(0);//1.0 = 150x150
        rarity_1->setPosition(convertFromDisplaySize(Point(x_point[2],footer_height + 4 * main_screen_size_split_5 + 20), 0, 0));
        this->addChild(rarity_1,10+card_index,TAG_RERITY);
        rarity_1->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f,ratio)));
    }
    if(rarity>=3){
        auto rarity_2 = Sprite::create("download/Gacha/rare3_150x150_2.png");//文字「激」
        rarity_2->setScale(0);//1.0 = 150x150
        rarity_2->setPosition(convertFromDisplaySize(Point(x_point[1],footer_height + 4 * main_screen_size_split_5 + 20), 0, 0));
        this->addChild(rarity_2,11+card_index,TAG_RERITY);
        rarity_2->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f,ratio)));
    }
    if(rarity>=2){
        auto rarity_3 = Sprite::create("download/Gacha/rare3_150x150_4.png");//文字「レ」
        rarity_3->setScale(0);//1.0 = 150x150
        rarity_3->setPosition(convertFromDisplaySize(Point(x_point[0],footer_height + 4 * main_screen_size_split_5 + 20), 0, 0));
        this->addChild(rarity_3,12+card_index,TAG_RERITY);
        rarity_3->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f,ratio)));
        
        auto rarity_4 = Sprite::create("download/Gacha/rare3_150x150_6.png");//文字「ア」
        rarity_4->setScale(0);//1.0 = 150x150
        rarity_4->setPosition(convertFromDisplaySize(Point(x_point[0]+120,footer_height + 4 * main_screen_size_split_5 + 20), 0, 0));
        this->addChild(rarity_4,13+card_index,TAG_RERITY);
        rarity_4->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f,ratio)));
    }
}

void GachaResultAnimationLayer::animationEnd(){
    /*
    if(touch_end){
        allTapDisaleRemove();
        this->removeFromParent();
    }
    if(_card_count==10){
        if(roop_count==9){
            allTapDisaleRemove();
            this->removeFromParent();
        }else{
            roop_count++;
        }
    }else{
        allTapDisaleRemove();
        this->removeFromParent();
    }
     */
    
    if(_animation_end == false){
        if(_card_count==1){
            if(_box_gacha_flg==false) {
                ((NormalScene*)this->getParent())->changePage("GachaMenu") ;
            } else {
                ((NormalScene*)this->getParent())->changePage("BoxGacha") ;
            }
            allTapDisaleRemove();
            this->removeFromParent();
        }else{
            _parent->scheduleOnce(_sel, 0.0f);
            this->runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create(CC_CALLBACK_0(GachaResultAnimationLayer::allTapDisaleRemove,this)), RemoveSelf::create(), nullptr)) ;
        }
            
    }
    _animation_end = true ;
}

void GachaResultAnimationLayer::setScheduler(Layer *parent, SEL_SCHEDULE sel)
{
    _parent = parent;
    _sel = sel;
}

void GachaResultAnimationLayer::allTapDisaleRemove(){
    auto normal_scene = (NormalScene *)this->getParent();
    if (!normal_scene) {
        return;
    }
    auto allTapDisaleLayer = (Layer*)normal_scene->getChildByTag(ALL_TAP_DISABLE_LAYER_TAG);
    allTapDisaleLayer->removeFromParent();
}
void GachaResultAnimationLayer::createColorSprite(){
    //真っ白な画面いっぱいのスプライト
    _white_sprite = Sprite::create();
    _white_sprite->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    _white_sprite->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    _white_sprite->setColor(Color3B::WHITE);
    _white_sprite->setOpacity(255) ;
    _white_sprite->setVisible(false) ;
    this->addChild(_white_sprite,99999);
    
    //真っ黒なスプライトを画面いっぱいにかぶせる 初期状態は透明
    _black_sprite = Sprite::create();
    _black_sprite->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    _black_sprite->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    _black_sprite->setColor(Color3B::BLACK);
    _black_sprite->setVisible(false) ;
    this->addChild(_black_sprite,99999);
}

void GachaResultAnimationLayer::startWhiteFadeOut(){
    _white_sprite->runAction(Sequence::create(DelayTime::create(2.0f), Show::create(), FadeOut::create(0.5f), Hide::create() ,nullptr)) ;
}

void GachaResultAnimationLayer::onEnter(){
    Layer::onEnter();
    
    //サイズの取得
    getSize();
    
    //背景画像の作成
    createBg();
    
    //演出中利用する色がついたスプライトを生成
    createColorSprite() ;
    
    //ガチャを引いたアニメーションの後のつなぎ目の白い画面のフェードアウト
    startWhiteFadeOut() ;
    
    //リザルトのアニメーションのフロー開始
    this->schedule(schedule_selector(GachaResultAnimationLayer::updateForResultFlow), 2.5);
    
}

void GachaResultAnimationLayer::setCharaDetailByIndex(int index){
    
    //キャラ詳細スプライトの作成
    _chara_detail_sprite = CharaDetailSpriteForResultAnimation::create(_chara_list_id[index]) ;
    _chara_detail_sprite->setScale(0) ;
//        _chara_detail_sprite->setPosition(convertFromDisplaySize(Point((main_screen_w / 2),footer_height + 2.5 * main_screen_size_split_5), 0, 0));
    _chara_detail_sprite->setPosition(convertFromDisplaySize(Point((main_screen_w / 2),footer_height + 1.5 * main_screen_size_split_5), 0, 0));
    this->addChild(_chara_detail_sprite,5+index);
}

void GachaResultAnimationLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GachaResultAnimationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GachaResultAnimationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GachaResultAnimationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(GachaResultAnimationLayer::onTouchCancelled, this);
    listener->setSwallowTouches(true);

    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
bool GachaResultAnimationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_touch_enable == false) return false  ;
    
    //一定間隔タッチ不可能にする
    _touch_enable = false ;
    this->schedule(schedule_selector(GachaResultAnimationLayer::touchIntervalCount), 0.3);
    
    return true ;
}
void GachaResultAnimationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void GachaResultAnimationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
    //まだアクションの再生中であった場合、タップにより詳細を強制的に表示する
    if(_now_animation){
        resultActionForceEnd(_now_card_index) ;
    }else{
        if(_card_count != _now_card_index){
            updateForResultFlow(0.0f) ;
        }else{
            animationEnd();
        }
    }
    
}

void GachaResultAnimationLayer::touchIntervalCount(float dt){
    _touch_enable = true;
}

void GachaResultAnimationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}