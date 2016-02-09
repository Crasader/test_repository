#include "DailyMissionGroupSprite.h"
#include "NormalScene.h"

DailyMissionGroupSprite* DailyMissionGroupSprite::create(DailyMissionGroupData *daily_mission_group_data)
{
    DailyMissionGroupSprite *sprite = new DailyMissionGroupSprite();
    
    if (sprite && sprite->init(daily_mission_group_data))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DailyMissionGroupSprite::init(DailyMissionGroupData *daily_mission_group_data)
{
    if ( !Sprite::initWithFile("init/Home/DailyMission/beginner_bk_s.png"))
    {
        return false;
    }
    CCLOG("DailyMissionGroupSprite::init()");
    
    _daily_mission_group_data = daily_mission_group_data;
    this->addChild(daily_mission_group_data) ;
    
    return true;
}

void DailyMissionGroupSprite::onEnter()
{
    
    createTitle() ;
    createReward() ;
    createConditionValue() ;
    
    //点滅する枠を表示
    createFlasingFrame() ;
    
    Sprite::onEnter();
}

void DailyMissionGroupSprite::createFlasingFrame()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        AchievementData* data = *iterator ;
        int index = (int)(iterator - achivement_data_list.begin()) ;
        if(!(data->getIsOldClear() || data->getIsNewClear() || data->getIsAlreadyDisplayedStamp())){
            //奥の光の枠を生成
            Sprite *flasing_frame_back = Sprite::create("init/Home/DailyMission/panel_light.png") ;
            flasing_frame_back->setPosition(Point(basePositionXByIndex(index) , 180)) ;
            this->addChild(flasing_frame_back) ;
            
            //手前の点滅する光の枠を生成
            Sprite *flasing_frame_front_blink = Sprite::create("init/Home/DailyMission/panel_light2.png") ;
            flasing_frame_front_blink->runAction(RepeatForever::create(Sequence::create(EaseSineIn::create(FadeIn::create(0.7)),EaseSineOut::create(FadeOut::create(0.7)), nullptr))) ;//点滅のアクション
            flasing_frame_front_blink->setAnchorPoint(Point::ZERO) ;
            flasing_frame_back->addChild(flasing_frame_front_blink) ;
            
            return ;
        }
    }
}

bool DailyMissionGroupSprite::hasNextStampAnimation()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        AchievementData* data = *iterator ;
        if(data->getIsNewClear() && data->getIsAlreadyDisplayedStamp() == false){
            return true ;
        }
    }
    return false ;
}

void DailyMissionGroupSprite::nextStampAnimation(Layer *parent, SEL_SCHEDULE end_scheduler)
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        int index = (iterator - achivement_data_list.begin()) ;
        
        AchievementData* data = *iterator ;
        if(data->getIsNewClear() && data->getIsAlreadyDisplayedStamp() == false){
            data->setIsAlreadyDisplayedStamp(true) ;
            startStampAnimation(parent, end_scheduler, data, Point(basePositionXByIndex(index), 170)) ;
            return ;
        }
    }
}

void DailyMissionGroupSprite::startStampAnimation(Layer* parent, SEL_SCHEDULE end_scheduler, AchievementData* achievement_data, Point center_point){

    //タップ禁止レイヤー生成
    auto normal_scene = (NormalScene *)this->getParent()->getParent()->getParent()->getParent()->getParent() ;
    normal_scene->addTapDisableLayer() ;

    Sprite *stamp = Sprite::create("init/Home/DailyMission/clear.png") ;
    //アニメーションが再生される最初の値を設定する
    stamp->setOpacity(0) ; //透過度を0にして見えないようにしておく
    stamp->setScale(4) ;
    stamp->setRotation(20) ;
    stamp->setPosition(center_point) ;
    this->addChild(stamp,10) ;
    
    //スタンプが押されるアニメーション
    ActionInterval *stamp_action = createStampAction() ;

    auto next_animation_func = CallFunc::create([parent, end_scheduler, this, normal_scene, achievement_data](){
        
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer() ;
        
        //アニメーションを再生するための情報の取得
        string reward_item_type  = achievement_data->getItemType() ;
        string reward_related_id = achievement_data->getRelatedPrimaryId() ;
        string comment           = achievement_data->getClearComment() ; ;
        string count             = achievement_data->getCount() ;
        string chara_name        = achievement_data->getCharaName() ;

        //別レイヤーでアニメーションを再生（通常の達成レイヤーの表示）
        auto layer = DailyMissionClearAnimLayer::create(reward_item_type, reward_related_id, comment, count, chara_name, false);
        normal_scene->addChild(layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
        
        //閉じられたときによばれる関数の指定
        layer->setSchedulerForClose(parent, end_scheduler);
    });

    stamp->runAction(Sequence::create(DelayTime::create(1.0), stamp_action, DelayTime::create(1.0), next_animation_func, nullptr)) ;

}

void DailyMissionGroupSprite::createTitle()
{
    //タイトル
    Sprite *title_image = Sprite::create(_daily_mission_group_data->getTitleImage()) ;
    title_image->setPosition(Point(this->getContentSize().width/2, 282)) ;
    this->addChild(title_image) ;
}

void DailyMissionGroupSprite::createReward()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        int index = (int)(iterator - achivement_data_list.begin()) ;
        
        AchievementData* data = *iterator ;
        
        string item_type = data->getItemType() ;
        string related_primary_id = data->getRelatedPrimaryId() ;
        string count = data->getCount() ;
        
        Sprite *sprite = getRewardSprite(item_type, related_primary_id) ;
        sprite->setPosition(Point(basePositionXByIndex(index) , 150)) ;
        this->addChild(sprite) ;
        
        if(count != "" && count !="0"){
            //個数の表示
            auto count_label = HMSFontUtil::createWithTTFBySystemFontArgument("×"+count, FONT_NAME_SYSTEM, 20);
            count_label->setHorizontalAlignment(TextHAlignment::CENTER);
            count_label->setAnchorPoint(Point(0.5f, 0.0f));
            count_label->setPosition(Point(basePositionXByIndex(index) , 110));
            this->addChild(count_label, 2);
        }
    }
}



void DailyMissionGroupSprite::createConditionValue()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        int index = (int)(iterator - achivement_data_list.begin()) ;
        AchievementData* data = *iterator ;
        
        string clear_condition_value = data->getClearConditionValue()  ;
        Sprite *sprite = createCountSprite(clear_condition_value, _daily_mission_group_data->getCountImage()) ;
        sprite->setPosition(Point(basePositionXByIndex(index), 218)) ;
        sprite->setScale(1.0f, -1.0f) ;
        if(sprite->getContentSize().width>75){
            float rate = sprite->getContentSize().width/75 ;
            sprite->setScaleX(sprite->getScaleX()/rate) ;
            sprite->setScaleY(sprite->getScaleY()/rate) ;
        }
        this->addChild(sprite) ;
    }
    
}

Sprite* DailyMissionGroupSprite::createCountSprite(string number, string unit) //unit=単位の意味で
{
    //左下基準を起点として配列にスプライトを入れていき、合成する
    vector<Sprite *> all_sprite;
    float offset_position_x = 0;
    for(int i=0; i<number.length(); i++) {
        string s = number.substr(i, 1); //数字を分解
        Sprite *num = Sprite::create("download/DailyMission/CountImages/num" + s + ".png");
        num->setAnchorPoint(Vec2(0.0f, 0.0f));
        num->setPosition(Point(offset_position_x, 0));
        all_sprite.push_back(num) ;
        offset_position_x+=num->getContentSize().width ;
    }
    
    //単位
    Sprite *unit_sprite = Sprite::create(unit);
    unit_sprite->setAnchorPoint(Vec2(0.0f, 0.0f));
    unit_sprite->setPosition(Point(offset_position_x, 0));
    all_sprite.push_back(unit_sprite);
    offset_position_x+=unit_sprite->getContentSize().width ;
    
    //高さと幅を調べる (横並びで合成)
    Size all_content_size = Size(0,0) ;
    for(vector<Sprite *>::iterator iterator = all_sprite.begin(); iterator != all_sprite.end(); iterator++){
        Sprite* sprite = *iterator ;
        all_content_size.width += sprite->getContentSize().width ;
        if(all_content_size.height <= sprite->getContentSize().height){
            all_content_size.height = sprite->getContentSize().height ;
        }
    }
    
    //合成開始
    auto render = RenderTexture::create(all_content_size.width, all_content_size.height);
    render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->begin();//書き込み開始
    for(vector<Sprite *>::iterator iterator = all_sprite.begin(); iterator != all_sprite.end(); iterator++){
        Sprite* sprite = *iterator ;
        sprite->visit() ;
    }
    render->end();//書き込み終了
    
    return Sprite::createWithTexture(render->getSprite()->getTexture()) ; //合成したものを返却
}

Sprite* DailyMissionGroupSprite::getRewardSprite(string item_type, string related_primary_id){
    
    Sprite *item_thumb = Sprite::create() ;
    
    if (item_type == "chara")
    {
        //カード画像をパネルの小さい画像の左上に重ねる
        string card_pass = "download/Chara/all100/" + related_primary_id + ".png";
        item_thumb->setTexture(card_pass);
        item_thumb->setScale(0.6) ;
    }
    else if (item_type == "jewel")
    {
        item_thumb->setTexture("init/Home/DailyMission/reward1.png");
    }
    else if (item_type == "gold")
    {
        item_thumb->setTexture("init/Home/DailyMission/reward2.png");
    }
    else if (item_type == "exchange_ticket")
    {
        item_thumb->setTexture("init/Home/DailyMission/reward3.png");
    }
    
    return item_thumb ;
}


ActionInterval* DailyMissionGroupSprite::createStampAction(){
    ScaleTo *scaleTo = ScaleTo::create(1.3, 1) ;
    FadeTo *fadeTo = FadeTo::create(1.3, 255) ;
    RotateTo *rotateTo= RotateTo::create(1.3, 0) ;
    Spawn *spawn = Spawn::create(scaleTo, fadeTo, rotateTo, nullptr) ;
    EaseExponentialIn *ease_in = EaseExponentialIn::create(spawn) ;
    
    auto func = CallFunc::create([](){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/clear_tap.mp3");
    });
    auto seq = Sequence::create(ease_in, func, nullptr);
    
    return seq ;
}

//DailyMissionGroupSprite内の枠中心のx座標を0~4のindexから求める
float DailyMissionGroupSprite::basePositionXByIndex(int index){
   return this->getContentSize().width/2+(index-2)*118-1; //118は枠の中心間の間隔 150は背景の下端からの距離 最後の-1は画像が中心から+1ずれてるので
}


void DailyMissionGroupSprite::showAllOldClearStamp()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        AchievementData* data = *iterator ;
        int index = (int)(iterator - achivement_data_list.begin()) ;
        
        if(data->getIsOldClear()){
            Sprite *stamp = Sprite::create("init/Home/DailyMission/clear.png") ;
            stamp->setRotation(20) ;
            stamp->setPosition(Point(basePositionXByIndex(index), 170)) ;
            this->addChild(stamp,10) ;
        }
    }
}

void DailyMissionGroupSprite::showAllNewClearStamp()
{
    HmsVector<AchievementData*> achivement_data_list = _daily_mission_group_data->getAchievementDataList() ;
    for(vector<AchievementData *>::iterator iterator = achivement_data_list.begin(); iterator != achivement_data_list.end(); iterator++){
        AchievementData* data = *iterator ;
        int index = (int)(iterator - achivement_data_list.begin()) ;
        
        if(data->getIsNewClear()){
            Sprite *stamp = Sprite::create("init/Home/DailyMission/clear.png") ;
            stamp->setRotation(20) ;
            stamp->setPosition(Point(basePositionXByIndex(index), 170)) ;
            this->addChild(stamp,10) ;
        }
    }
}