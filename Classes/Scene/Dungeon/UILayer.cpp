/*HPなどの表示や、攻撃ボタンなどの表示と処理*/
#include "UILayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "DungeonSubMenu.h"
#include "TurnManage.h"
#include "StampLayer.h"
#include "CharaList.h"
#include "OutlineSprite.h"

#define TIME_GAUGE_TAG_ID 101

bool UILayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(UILayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(UILayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(UILayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}
void UILayer::initialize(){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    //UI背景
    back_ui=Sprite::create("init/Dungeon/design/battle_base_e.png");
     back_ui->setScale(ratio);
     back_ui->setAnchorPoint(Point(0.5, 0.0));
     back_ui->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
     this->addChild(back_ui,4);

    back_ui2=Sprite::create("init/Dungeon/design/battle_base_p.png");
     back_ui2->setScale(ratio);
     back_ui2->setAnchorPoint(Point(0.5, 1.0));
     back_ui2->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
     this->addChild(back_ui2,4);

    
/*
    auto test_sprite = OutlineSprite::createWithLabel(Label::createWithSystemFont("貫通　てすと　123456778", FONT_NAME_SYSTEM, 27*ratio), 2.0f*ratio);
    test_sprite->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(test_sprite,200);
    
    auto delay = DelayTime::create(0.08f);
    auto func_color = CallFunc::create([test_sprite](){
        ((OutlineSprite *)test_sprite)->outline->setColor(Color3B(0+random(0, 100),0+random(0, 100),150+random(0, 100)));
    
    });
    auto action = RepeatForever::create(Sequence::create(delay,func_color, NULL));
    ((OutlineSprite *)test_sprite)->outline->runAction(action);
*/
    
    //プレイヤー名
    for(int ii=0; ii<2; ii++) {
        
        if(ii==TurnManage::getEnemyIndex()){//敵
            power_point[ii] = Point(601,779);
            power_point1fig[ii] = Point(614,779);
            power_point2fig[ii] = Point(591,779);
            name_label[ii] = HMSFontUtil::createWithTTFBySystemFontArgument(dung->player[ii]->getName(), FONT_NAME_SYSTEM, 27*ratio);
            name_label[ii]->setPosition(convertFromDisplaySize(Point(135, 798), 0.5, 0.5));
        }else{//自分
            power_point[ii] = Point(601,167);
            power_point1fig[ii] = Point(614,167);
            power_point2fig[ii] = Point(591,167);
            name_label[ii] = HMSFontUtil::createWithTTFBySystemFontArgument(dung->player[ii]->getName(), FONT_NAME_SYSTEM, 27*ratio);
            name_label[ii]->setPosition(convertFromDisplaySize(Point(135, 160), 0.5, 0.5));
        }
        
        name_label[ii]->enableOutline(Color4B::BLACK);
        name_label[ii]->setAnchorPoint(Point(0, 0.5));
        addChild(name_label[ii], 100);
        
        
        //先攻か後攻か
        int ahead_index = static_cast<DungeonScene *>(getParent())->json_master["ahead_no"].int_value() - 1;
        if(ahead_index == ii){
            attack_order[ii] = Sprite::create("init/Dungeon/design/batting_first.png");
        }else{
            attack_order[ii] = Sprite::create("init/Dungeon/design/post_attack.png");
        }
        
        attack_order[ii]->setScale(ratio);
        attack_order[ii]->setAnchorPoint(Vec2(0,0.5));
        //attack_order[ii]->setPosition(name_label[ii]->getPosition() + name_label[ii]->getContentSize() + Vec2( 8*ratio, -31*ratio));
        attack_order[ii]->setPosition(name_label[ii]->getPosition().x + name_label[ii]->getContentSize().width +8*ratio, name_label[ii]->getPosition().y + 1*ratio);
        this->addChild(attack_order[ii],100);
    }

    //トーナメント残り時間
    if(dung->isTournamentEventBattle()){
        /*auto square = Sprite::create();
         square->setTextureRect(Rect(-50*ratio,-25*ratio,100*ratio,40*ratio));
         square->setPosition(convertFromDisplaySize(Point(70, 670), 0.5, 0.5));
         square->setColor(Color3B::BLACK);
         square->setOpacity(100);
         addChild(square, 120);*/
        
        
        rest_time_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 32*ratio);
        rest_time_label->setCascadeOpacityEnabled(true);
        //GlowとOutlineは同時に使えないらしいのでOutlineのみ
//        rest_time_label->enableGlow(Color4B::BLACK);
        rest_time_label->enableOutline(Color4B(0, 0, 0, 255), 4*ratio) ;
        rest_time_label->setPosition(convertFromDisplaySize(Point(70, 670), 0.5, 0.5));
        rest_time_label->setOpacity(255);
        this->addChild(rest_time_label,200);
        //auto scale1 = FadeTo::create(1.7f, 190);
        //auto scale2 = FadeTo::create(1.7f, 220);
        //auto ease1 = EaseSineInOut::create(scale1);
        //auto ease2 = EaseSineInOut::create(scale2);
        //rest_time_label->runAction(RepeatForever::create(Sequence::create(ease1,ease2, NULL)));
        rest_time_label->setVisible(false);
    }
    
    
    //攻撃ボタン
    attack_img = Sprite::create("init/Dungeon/design/battle_bt_attack.png");
    attack_img->setScale(ratio);
    attack_img->setPosition(convertFromDisplaySize(Point(576, 267), 0.5, 0.5));
    attack_img->setColor(Color3B(127,127,127));
    this->addChild(attack_img,6);
    
    //メニューボタン
    menu_img = Sprite::create("init/Dungeon/design/battle_bk_menu.png");
    menu_img->setScale(ratio);
    menu_img->setPosition(convertFromDisplaySize(Point(63, 292), 0.5, 0.5));
    this->addChild(menu_img,6);
    
    //スタンプボタン
    if(dung->getBattleMode() == "vs"){
        if(SaveData::loadInt("stamp_mute") == 1){ //ミュートの時
            stamp_img = Sprite::create("download/Dungeon/bt_emotion_off.png");
            stamp_img->setOpacity(160);
        }else{
            stamp_img = Sprite::create("download/Dungeon/bt_emotion_on.png");
            stamp_img->setOpacity(255);
        }
        stamp_img->setScale(ratio);
        stamp_img->setPosition(convertFromDisplaySize(Point(580, 690), 0.5, 0.5));
        this->addChild(stamp_img,6);
    }
    
    //士気ラベル
    for(int i=0; i<2; i++){
        numeric_power[i] = NumericCount::create();
        numeric_power[i]->setValue(0, "big");
        numeric_power[i]->setPosition(convertFromDisplaySize(power_point[i], 0.5, 0.5));
        addChild(numeric_power[i],6);
    }
    
    //HPゲージ
    for(int i = 0; i<2;i++){
        hpGuageFullSprite[i] = Sprite::create("init/Dungeon/design/battle_gauge_hp_green.png");
        hpGuageSprite[i] = Sprite::create("init/Dungeon/design/battle_gauge_hp_yellow.png");
        hpGuageRedSprite[i] = Sprite::create("init/Dungeon/design/battle_gauge_hp_red.png");
        hpGuageDangerSprite[i] = Sprite::create("init/Dungeon/design/battle_gauge_hp_red.png");
        
        hpGuageFullSprite[i]->setScale(ratio);
        hpGuageSprite[i]->setScale(ratio);
        hpGuageRedSprite[i]->setScale(ratio);
        hpGuageDangerSprite[i]->setScale(ratio);
        
        hpGuageFullSprite[i]->setAnchorPoint(Point(0, 0.5));
        hpGuageSprite[i]->setAnchorPoint(Point(0, 0.5));
        hpGuageRedSprite[i]->setAnchorPoint(Point(0, 0.5));
        hpGuageDangerSprite[i]->setAnchorPoint(Point(0, 0.5));
        
        if(i==TurnManage::getEnemyIndex()){
            hpGuageFullSprite[i]->setPosition(convertFromDisplaySize(Point(143,834), 0.5, 0.5));
            hpGuageSprite[i]->setPosition(convertFromDisplaySize(Point(143,834), 0.5, 0.5));
            hpGuageRedSprite[i]->setPosition(convertFromDisplaySize(Point(143,834), 0.5, 0.5));
            hpGuageDangerSprite[i]->setPosition(convertFromDisplaySize(Point(143,834), 0.5, 0.5));
        }else{
            hpGuageFullSprite[i]->setPosition(convertFromDisplaySize(Point(143,126), 0.5, 0.5));
            hpGuageSprite[i]->setPosition(convertFromDisplaySize(Point(143,126), 0.5, 0.5));
            hpGuageRedSprite[i]->setPosition(convertFromDisplaySize(Point(143,126), 0.5, 0.5));
            hpGuageDangerSprite[i]->setPosition(convertFromDisplaySize(Point(143,126), 0.5, 0.5));
        }
        
        hpGuageRedSprite[i]->setOpacity(100);
        hpGuageDangerSprite[i]->setOpacity(0);
        hpGuageSprite[i]->setOpacity(0);
        
        addChild(hpGuageFullSprite[i],1);
        addChild(hpGuageSprite[i],2);
        addChild(hpGuageRedSprite[i],0);
        addChild(hpGuageDangerSprite[i],3);
    }
    
    for(int i=0; i<2; i++){
        Rect rect = Rect(0,0,105,40);
        rectSprite[i] = Sprite::create();
        rectSprite[i]->setTextureRect(rect);
        rectSprite[i]->setColor(Color3B::RED);
        rectSprite[i]->setOpacity(0);
        rectSprite[i]->setScale(ratio);
        if(i==TurnManage::getEnemyIndex()){
            rectSprite[i]->setPosition(convertFromDisplaySize(Point(585,834), 0.5, 0.5));
        }else{
            rectSprite[i]->setPosition(convertFromDisplaySize(Point(585,126), 0.5, 0.5));
        }
        addChild(rectSprite[i],3);
        
        auto fade_in = FadeTo::create(0.4f, 130);
        auto fade_out = FadeTo::create(0.4f, 0);
        auto seq = Sequence::create(fade_in, fade_out, NULL);
        auto forever = Repeat::create(seq,1000);
        forever->setTag(100);
        rectSprite[i]->runAction(forever);
        rectSprite[i]->pause();
    }
    
    //タイマーゲージ
    for(int i=0; i<2; i++){
        timeGuageSprite[i]= Sprite::create("init/Dungeon/design/battle_gauge_time.png");
        timeGuageSprite[i]->setScale(ratio);
        timeGuageSprite[i]->setAnchorPoint(Point(0, 0.5));
        
        if(i==TurnManage::getEnemyIndex()) {
            timeGuageSprite[i]->setPosition(convertFromDisplaySize(Point(540,834), 0.5, 0.5));
        } else {
            timeGuageSprite[i]->setPosition(convertFromDisplaySize(Point(540,126), 0.5, 0.5));
        }
        addChild(timeGuageSprite[i],1);
    }
    
    DungeonScene *dungeon_scene = static_cast<DungeonScene *>(getParent());
    situation_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 28*ratio);
    situation_label->enableGlow(Color4B::BLACK);
    situation_label->enableOutline(Color4B::BLACK, 4*ratio);
    dungeon_scene->startBattleEffectLayer->addChild(situation_label, 1);
    
    scheduleUpdate();
}


void UILayer::timeAlert(bool flg, bool index){
    
    if(alert_flg[index] != flg){
        if(flg == true){//アラート起動
            HMSLOG("アラート起動");
            rectSprite[index]->resume();
        }else{//アラート停止
            rectSprite[index]->setOpacity(0);
            rectSprite[index]->pause();
            HMSLOG("アラート停止");
        }
    }else{
        if(flg ==false){
            rectSprite[index]->setOpacity(0);
            rectSprite[index]->pause();
        }
    }
    alert_flg[index] = flg;
}

void UILayer::setTimer(int percentage_list[], bool check_flg)
{
   // HMSLOG("タイマー:%d ,チェックフラグ:%d",percentage_list[0],(int)check_flg);
   // HMSLOG("タイマー2:%d",percentage_list[1]);
    DungeonScene *dungeon = static_cast<DungeonScene *>(getParent());
    string str = "";
    if(check_flg==false) {
        str = "_yellow";
    }
    
    //タイムゲージ
    for(int i=0; i<2; i++){
        string filename;
        int percentage = percentage_list[i];;
        
        if(check_flg == false) {
            percentage = 100;
            name_label[i]->setColor(Color3B::RED);
        }else{
            name_label[i]->setColor(Color3B::WHITE);
        }
        
        if(percentage < 0){
            timeAlert(false, i);
            filename = "init/Dungeon/design/battle_gauge_time_yellow.png";
            percentage = 0;
            if(i == TurnManage::getPlayerIndex() && dungeon->isCharacterPlacable()){//自分のターンに自分のタイムがなくなった時
                dungeon->attack(TurnManage::getPlayerIndex(), true);//攻撃しないでターン終了
            }
            
        }else if(percentage < 1){
            timeAlert(false, i);
            filename = "init/Dungeon/design/battle_gauge_time_yellow.png";
        }else if(percentage < 25){
            filename = "init/Dungeon/design/battle_gauge_time_yellow.png";
            timeAlert(true, i);
        }else if(percentage < 50){
            filename = "init/Dungeon/design/battle_gauge_time_yellow.png";
            timeAlert(false, i);
        }else{
            filename = "init/Dungeon/design/battle_gauge_time.png";
            timeAlert(false, i);
        }
        
        timeGuageSprite[i]->setTexture(filename.c_str());
        timeGuageSprite[i]->setScale(ratio * percentage / 100.0f,ratio);
    }
    
    //敵の動向メッセージ
    DungeonScene *dungeon_scene = static_cast<DungeonScene *>(getParent());
    string str2 = "";
    
    if(TurnManage::getTurnNo()==0) return;//最初のセットターンが終わるまでは何もしない
    if(dungeon_scene->replay_flg==true) return;//リプレイでは表示しない
    
    if(!dungeon_scene->isMyTurn() && dungeon_scene->state!="attack_effect" && dungeon_scene->data_exec_flg==false){
        str2 = "相手プレイヤーが思考中です";
        for(int i=0; i<dot_count; i++) {
            str2 += ".";
        }
    }else{
        dot_count = 0;
    }
    if(dungeon_scene->connection_flg[TurnManage::getEnemyIndex()]==0) {
        if(TurnManage::getTurnCount() >= 3){
            str2 = "相手プレイヤーの接続が切れました";
        }else{
            str2 = "相手プレイヤーは準備中です";
        }
    }
    
    
    if(situation_label->getString() != str2){
        situation_label->setString(str2);
    }
    
    if(dungeon_scene->connection_flg[TurnManage::getEnemyIndex()]==0) {
        situation_label->setPosition(convertFromDisplaySize(Point(320, 904), 0.5, 0.5));
        situation_label->setAnchorPoint(Point(0.5, 0.5));
    }else{
        situation_label->setPosition(convertFromDisplaySize(Point(140, 904), 0.5, 0.5));
        situation_label->setAnchorPoint(Point(0, 0.5));
    }
    
    dot_count++;
    if(dot_count>3) dot_count = 0;
    
}

void UILayer::updatePower(int now_power, int turn_index){
    
    numeric_power[turn_index]->setPosition(convertFromDisplaySize(power_point[turn_index], 0.5, 0.5));
    numeric_power[turn_index]->setValue(now_power, "big");
    
}

void UILayer::updatePlayerInfo(int hp, int hp_max, int index){
    DungeonScene *dungeon = static_cast<DungeonScene *>(getParent());
    int y = 126;
    if(index==TurnManage::getEnemyIndex()) y = 960 - 126;
    
    
    
    if(hp_label[index] != nullptr){//最初の何も入っていない時
        removeChild(hp_label[index]);
    }
    if(hp_labelL[index] != nullptr){//最初の何も入っていない時
        removeChild(hp_labelL[index]);
    }
    
    hp_labelL[index] = OutlineSprite::createWithLabel(Label::createWithTTF(to_string(hp),FONT_NAME,23*ratio),2.0f*ratio);
    
    hp_label[index] = OutlineSprite::createWithLabel(Label::createWithTTF("/" + to_string(hp_max),FONT_NAME,23*ratio),2.0f*ratio);
    
    ((OutlineSprite *)hp_labelL[index])->outline->setColor(Color3B::BLACK);
    ((OutlineSprite *)hp_label[index])->outline->setColor(Color3B::BLACK);
   
    
    if(hp > 0){
        hpGuageFullSprite[index]->setScale((float)hp / hp_max * ratio, ratio);//HPゲージ幅更新
        hpGuageSprite[index]->setScale((float)hp / hp_max * ratio, ratio);//HPゲージ幅更新
        hpGuageDangerSprite[index]->setScale((float)hp / hp_max * ratio, ratio);//HPゲージ幅更新
        
        if(hp >= hp_max * 5.0f / 10.0f){//５割以上
            hpGuageFullSprite[index]->setVisible(true);
            hpGuageSprite[index]->setVisible(true);
            hpGuageDangerSprite[index]->setVisible(false);
        
            hpGuageSprite[index]->setOpacity( (1.0f - ((((float)hp / (float)hp_max) - 0.5f) * 2.0f))  * 250.0f );
            
        }else if(hp < hp_max * 5.0f / 10.0f){//５割未満
            hpGuageFullSprite[index]->setVisible(false);
            hpGuageSprite[index]->setVisible(true);
            hpGuageDangerSprite[index]->setVisible(true);
            
            hpGuageSprite[index]->setOpacity(255);
            hpGuageDangerSprite[index]->setOpacity( (1 - (float)hp / (hp_max * 5.0f / 10.0f)) *250 );
            
        }else{
            hpGuageDangerSprite[index]->setVisible(false);
        }
        
        
    }else{//HPが０の時
        CharaList::getLeader(index)->eraseLeader();//総大将死亡エフェクト
        dungeon->game_end_flg = true;;
        hpGuageFullSprite[index]->setScale((float)0.0f / hp_max * ratio, ratio);//HPゲージ幅更新
        hpGuageSprite[index]->setScale((float)0.0f / hp_max * ratio, ratio);//HPゲージ幅更新
        hpGuageDangerSprite[index]->setScale((float)0.0f / hp_max * ratio, ratio);//HPゲージ幅更新
        //hp_labelL[index]->setString(to_string(0));
        //hp_label[index]->setString("/" + to_string(hp_max));
    }
    addChild(hp_labelL[index], 50, 4);
    addChild(hp_label[index], 50, 6);
    
    auto scale = ScaleTo::create(0.7f, (float)hp / hp_max * ratio, ratio);
    hpGuageRedSprite[index]->runAction(scale);
    
    hp_label[index]->setPosition(convertFromDisplaySize(Point(405 + ((OutlineSprite *)hp_label[index])->label->getContentSize().width/ratio/2, y), 0.5, 0.5));
    hp_labelL[index]->setPosition(convertFromDisplaySize(Point(405 - ((OutlineSprite *)hp_labelL[index])->label->getContentSize().width/ratio/2, y), 0.5, 0.5));
    
    
    if((float)hp/hp_max > 0.1f){//一割より多い
        
        
        //hp_labelL[index]->enableOutline(Color4B::BLACK, 4*ratio);
        ((OutlineSprite *)hp_labelL[index])->outline->stopAllActions();//点滅アクション停止
        hp_labelL[index]->setOpacity(255);
    }else if((float)hp/hp_max > 0.0f){//一割以下
        //hp_labelL[index]->enableOutline(Color4B::RED, 4*ratio);
        //auto fade1 = FadeTo::create(0.8f, 255);
        //auto fade2 = FadeTo::create(0.8f, 170);
        auto tint1 = TintTo::create(0.6f, 255, 0, 0);
        auto tint2 = TintTo::create(0.6f, 0, 0, 0);
        
        
        auto repeat = RepeatForever::create(Sequence::create(tint1,tint2, NULL));
        ((OutlineSprite *)hp_labelL[index])->outline->runAction(repeat);
    }else{//ゼロ
        ((OutlineSprite *)hp_labelL[index])->outline->setColor(Color3B::RED);
    }
}

void UILayer::insertPassiveIcon(int index, int primary, int skill_id){
    
    Sprite *sprite =static_cast<DungeonScene *>(getParent())->passiveSkill->getIcon(skill_id);
    if(sprite !=nullptr){
        
        passiveBufferIcon[index].push_back(make_pair(primary,sprite));
        sprite->setAnchorPoint(Point(0.5,0.5));
        sprite->setScale(ratio);
        sprite->setPosition(getIconPosition(sprite));
        addChild(sprite,200);
        
        ScaleTo *scale = ScaleTo::create(0.3,1.7*ratio);
        EaseOut *ease_o = EaseOut::create(scale, 2);
        
        
        DelayTime *delay = DelayTime::create(0.2);
        ScaleTo *scale2 = ScaleTo::create(0.6, 0.75*ratio);
        EaseIn *ease_i = EaseIn::create(scale2, 2);
        
        Sequence *seq = Sequence::create(ease_o,delay,ease_i,CallFunc::create([this,index,sprite](){
            this->reorderChild(sprite, 100+(int)(passiveBufferIcon[index].size())-1);
        }), nullptr);
        
        sprite->runAction(seq);
        
        /*runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([this](){
            resetIconPosition();
        }), NULL));*/
    
    }else{
        HMSLOG("UIアイコンえらー");
    }
}

void UILayer::removePassiveIcon(int primary){
    for(int ii=0; ii<2; ii++){
        auto itr = passiveBufferIcon[ii].begin();
        while (itr != passiveBufferIcon[ii].end()) {
            if((*itr).first == primary){
                ScaleTo *scale = ScaleTo::create(0.3,1.2*ratio);
                EaseOut *scale_o = EaseOut::create(scale, 2);
                //RotateTo *rotate =RotateTo::create(1.0,220);
                FadeOut *fade = FadeOut::create(0.8);
                ScaleTo *scale2 = ScaleTo::create(0.8,0.6*ratio);
                //EaseIn *scale_i = EaseIn::create(scale2,2);
                Spawn *spawn = Spawn::create(fade,scale2,nullptr);
                
                RemoveSelf *remove = RemoveSelf::create();
                Sequence *seq = Sequence::create(scale_o,spawn,remove,nullptr);
                (*itr).second->runAction(seq);
                
                passiveBufferIcon[ii].erase(itr);
            }else{
                itr++;
            }
        }
    }
    
    
    /*for(auto itr = passiveBufferIcon[index].begin(); itr!=passiveBufferIcon[index].end(); itr++){
        if((*itr).first == primary){
            
            ScaleTo *scale = ScaleTo::create(0.3,1.2*ratio);
            EaseOut *scale_o = EaseOut::create(scale, 2);
            //RotateTo *rotate =RotateTo::create(1.0,220);
            FadeOut *fade = FadeOut::create(0.8);
            ScaleTo *scale2 = ScaleTo::create(0.8,0.6*ratio);
            //EaseIn *scale_i = EaseIn::create(scale2,2);
            Spawn *spawn = Spawn::create(fade,scale2,nullptr);
            
            RemoveSelf *remove = RemoveSelf::create();
            Sequence *seq = Sequence::create(scale_o,spawn,remove,nullptr);
            (*itr).second->runAction(seq);
            
            //removeChild(passiveBufferIcon[index].at(i).second);
            passiveBufferIcon[index].erase(itr);
            //resetIconPosition();
            break;
        }
    }*/
}

void UILayer::insertWholeBufferIcon(int index, Chara *used_chara ,int icon_id){
    
    Sprite *sprite;
    if(icon_id > 0){
        sprite =static_cast<DungeonScene *>(getParent())->activeSkill->getIcon(icon_id);
    }else{
        sprite =static_cast<DungeonScene *>(getParent())->activeSkill->getIcon(used_chara->getActiveSkillId());
    }
 
    if(sprite !=nullptr){
        
        wholeBufferIcon[index].push_back(make_pair(used_chara->getPrimaryId(),sprite));
        sprite->setAnchorPoint(Point(0.5,0.5));
        sprite->setScale(ratio);
        sprite->setPosition(getIconPosition(sprite));
        addChild(sprite,200);
        ScaleTo *scale = ScaleTo::create(0.3,1.8*ratio);
        DelayTime *delay = DelayTime::create(0.3);
        ScaleTo *scale2 = ScaleTo::create(0.6, 0.75*ratio);
        Sequence *seq = Sequence::create(scale,delay,scale2,CallFunc::create([this,index,sprite](){
            this->reorderChild(sprite, 100+(int)passiveBufferIcon[index].size()+(int)wholeBufferIcon[index].size()-1);
        }), nullptr);
        
        sprite->runAction(seq);
        
        resetIconPosition();
    }else{
        HMSLOG("UIアイコンえらー");
    }
}

void UILayer::removeWholeBufferIcon(int primary){
    for(int ii=0; ii<2; ii++){
        auto itr = wholeBufferIcon[ii].begin();
        while (itr != wholeBufferIcon[ii].end()) {
            if((*itr).first == primary){
                ScaleTo *scale = ScaleTo::create(1.0,0.2*ratio);
                RotateBy *rotate =RotateBy::create(1.0,270);
                FadeOut *fade = FadeOut::create(1.0);
                RemoveSelf *remove = RemoveSelf::create();
                Spawn *spa = Spawn::create(scale,rotate,fade, nullptr);
                CallFunc *func =CallFunc::create([this](){
                    resetIconPosition();
                });
                Sequence *seq = Sequence::create(spa,remove,func,nullptr);
                (*itr).second->runAction(seq);
                
                //removeChild(wholeBufferIcon[index].at(i).second);
                wholeBufferIcon[ii].erase(itr);
            }else{
                itr++;
            }
        }
    }
    
    /*for(auto itr = wholeBufferIcon[index].begin(); itr!=wholeBufferIcon[index].end(); itr++){
        if((*itr).first == primary){
            
            ScaleTo *scale = ScaleTo::create(1.0,0.2*ratio);
            RotateBy *rotate =RotateBy::create(1.0,270);
            FadeOut *fade = FadeOut::create(1.0);
            RemoveSelf *remove = RemoveSelf::create();
            Spawn *spa = Spawn::create(scale,rotate,fade, nullptr);
            CallFunc *func =CallFunc::create([this](){
                resetIconPosition();
            });
            Sequence *seq = Sequence::create(spa,remove,func,nullptr);
            (*itr).second->runAction(seq);
            
            //removeChild(wholeBufferIcon[index].at(i).second);
            wholeBufferIcon[index].erase(itr);
            
            break;
        }
    }*/
}

void UILayer::resetShowAllIcon(){//すべてのアイコンを一旦消し、バッファリストを見て最表示
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<wholeBufferIcon[ii].size(); i++){
            removeChild(wholeBufferIcon[ii].at(i).second);
        }
        wholeBufferIcon[ii].clear();
    }
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<passiveBufferIcon[ii].size(); i++){
            removeChild(passiveBufferIcon[ii].at(i).second);
        }
        passiveBufferIcon[ii].clear();
    }
    
    
    DungeonScene *dungeon = static_cast<DungeonScene *>(getParent());
    
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<dungeon->player[ii]->whole_status_passive.size(); i++){
            for(Chara *chara : CharaList::getAll()){
                if(chara->getPrimaryId() == dungeon->player[ii]->whole_status_passive.at(i)["primary"]){
                    Sprite *sprite = dungeon->passiveSkill->getIcon(chara->getPassiveSkillId());
                    if(sprite !=nullptr){
                        passiveBufferIcon[ii].push_back(make_pair(dungeon->player[ii]->whole_status_passive.at(i)["primary"],sprite));
                        sprite->setAnchorPoint(Point(0.5,0.5));
                        sprite->setScale(0.75*ratio);
                        //getIconPosition(sprite);
                        addChild(sprite,100+i);
                    }
                }
            }
        }
    }
    
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<dungeon->player[ii]->whole_status_active.size(); i++){
            for(Chara *chara : CharaList::getAll()){
                if(chara->getPrimaryId() == dungeon->player[ii]->whole_status_active.at(i)["primary"]){
                    Sprite *sprite = dungeon->activeSkill->getIcon(chara->getActiveSkillId());
                    if(sprite !=nullptr){
                        wholeBufferIcon[ii].push_back(make_pair(dungeon->player[ii]->whole_status_active.at(i)["primary"],sprite));
                        sprite->setAnchorPoint(Point(0.5,0.5));
                        sprite->setScale(0.75*ratio);
                        //getIconPosition(sprite);
                        addChild(sprite,100+i+(int)dungeon->player[ii]->whole_status_passive.size());
                    }
                }
            }
        }
    }
    
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<wholeBufferIcon[ii].size();i++){
            wholeBufferIcon[ii].at(i).second->setPosition(getIconPosition(wholeBufferIcon[ii].at(i).second));
        }
    }
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<passiveBufferIcon[ii].size();i++){
            passiveBufferIcon[ii].at(i).second->setPosition(getIconPosition(passiveBufferIcon[ii].at(i).second));
        }
    }
    
    resetIconPosition();
}

void UILayer::resetIconPosition(){
    CCLOG("リセットアイコンポジション");
    
    //味方のアイコン
    for(int i =0; i<passiveBufferIcon[TurnManage::getPlayerIndex()].size();i++){
        auto move = MoveTo::create(0.5f, getIconPosition(passiveBufferIcon[TurnManage::getPlayerIndex()].at(i).second));
        move->setTag(777);
        passiveBufferIcon[TurnManage::getPlayerIndex()].at(i).second->stopActionByTag(777);
        passiveBufferIcon[TurnManage::getPlayerIndex()].at(i).second->runAction(EaseSineOut::create(move));
    }
    for(int i =0; i<wholeBufferIcon[TurnManage::getPlayerIndex()].size();i++){
        auto move = MoveTo::create(0.5f, getIconPosition(wholeBufferIcon[TurnManage::getPlayerIndex()].at(i).second));
        move->setTag(777);
        wholeBufferIcon[TurnManage::getPlayerIndex()].at(i).second->stopActionByTag(777);
        wholeBufferIcon[TurnManage::getPlayerIndex()].at(i).second->runAction(EaseSineOut::create(move));
    }
    
    //敵のアイコン
    for(int j =0; j<passiveBufferIcon[TurnManage::getEnemyIndex()].size();j++){
        auto move = MoveTo::create(0.5f, getIconPosition(passiveBufferIcon[TurnManage::getEnemyIndex()].at(j).second));
        move->setTag(777);
        passiveBufferIcon[TurnManage::getEnemyIndex()].at(j).second->stopActionByTag(777);
        passiveBufferIcon[TurnManage::getEnemyIndex()].at(j).second->runAction(EaseSineOut::create(move));
    }
    for(int j =0; j<wholeBufferIcon[TurnManage::getEnemyIndex()].size();j++){
        auto move = MoveTo::create(0.5f, getIconPosition(wholeBufferIcon[TurnManage::getEnemyIndex()].at(j).second));
        move->setTag(777);
        wholeBufferIcon[TurnManage::getEnemyIndex()].at(j).second->stopActionByTag(777);
        wholeBufferIcon[TurnManage::getEnemyIndex()].at(j).second->runAction(EaseSineOut::create(move));
    }
}

Vec2 UILayer::getIconPosition(Sprite *icon_sprite){
    
    int interval_p = 48;//間隔
    int icon_count_p = (int)passiveBufferIcon[TurnManage::getPlayerIndex()].size() + (int)wholeBufferIcon[TurnManage::getPlayerIndex()].size();//アイコン数
    if(icon_count_p > 2)interval_p -= icon_count_p * 2 - 4;


    int interval_e = 48;//間隔
    int icon_count_e = (int)passiveBufferIcon[TurnManage::getEnemyIndex()].size() + (int)wholeBufferIcon[TurnManage::getEnemyIndex()].size();//アイコン数
    if(icon_count_e > 2)interval_e -= icon_count_e * 2 - 4;


    for(int ii=0; ii<2; ii++){
        for(int i=0; i<passiveBufferIcon[ii].size(); i++){
            if(passiveBufferIcon[ii].at(i).second == icon_sprite){//そのスプライトがパッシブのiこ目と一致したら
                if(ii == TurnManage::getPlayerIndex()){
                    //CCLOG("pアイコンポジション：%d：%d",(int)convertFromDisplaySize(Point(529-interval_p*i,163),0.5,0.5).x, (int)convertFromDisplaySize(Point(529-interval_p*i,163),0.5,0.5).y);
                    return convertFromDisplaySize(Point(529-interval_p*i,163),0.5,0.5);
                }else{
                    return convertFromDisplaySize(Point(529-interval_e*i,800),0.5,0.5);
                }
            }
        }
        for(int i=0; i<wholeBufferIcon[ii].size(); i++){
            if(wholeBufferIcon[ii].at(i).second == icon_sprite){
                if(ii == TurnManage::getPlayerIndex()){
                    //CCLOG("aアイコンポジション：%d：%d",(int)convertFromDisplaySize(Point(529-interval_p*(i+passiveBufferIcon[TurnManage::getPlayerIndex()].size()),163),0.5,0.5).x, (int)convertFromDisplaySize(Point(529-interval_p*(i+passiveBufferIcon[TurnManage::getPlayerIndex()].size()),163),0.5,0.5).y);
                    //CCLOG("アイコンテスト：%d,%d,%d",interval_p,i,(int)passiveBufferIcon[TurnManage::getPlayerIndex()].size());
                    return convertFromDisplaySize(Point(529-interval_p*(i+passiveBufferIcon[TurnManage::getPlayerIndex()].size()),163),0.5,0.5);
                }else{
                    return convertFromDisplaySize(Point(529-interval_e*(i+passiveBufferIcon[TurnManage::getEnemyIndex()].size()),800),0.5,0.5);
                }
            }
        }
    }
    return Vec2(0,0);
}

void UILayer::updateBufferIcon(){
    
    //アイコンスプライトの取得
    for(int i =0;i<2;i++){
        for(int j=0;j<wholeBufferIcon[i].size();j++){
            
            FadeIn *fadeIn = FadeIn::create(0.6);
            DelayTime * delay = DelayTime::create(0.3);
            FadeTo *fadeOut = FadeTo::create(0.6,150);
            //RemoveSelf *remove = RemoveSelf::create();
            
            Sequence *seq = Sequence::create(fadeIn,delay,fadeOut/*,remove*/, nullptr);
            wholeBufferIcon[i].at(j).second ->runAction(seq);
        }
    }
}

void UILayer::changeAttackButton(string state){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    auto old_color = attack_img->getColor();
    
    if(attack_img == nullptr){
        attack_img = Sprite::create("init/Dungeon/design/battle_bt_waiting.png");
        this->addChild(attack_img,6);
    }
    
    
    
    if(state == "attack"){
        if(dung->isMyTurn()){
            if(TurnManage::getTurnCount() == 1){
                attack_img->setTexture("init/Dungeon/design/battle_bt_waiting.png");
                attack_img->setScale(ratio);
                attack_img->setPosition(convertFromDisplaySize(Point(576, 267), 0.5, 0.5));
                attack_img->setColor(Color3B(255,255,255));
            }else{
                attack_img->setTexture("init/Dungeon/design/battle_bt_attack.png");
                attack_img->setScale(ratio);
                attack_img->setPosition(convertFromDisplaySize(Point(576, 267), 0.5, 0.5));
                attack_img->setColor(old_color);
            }
            //使用できる士気コストで何も出来ない時、攻撃ボタンをアクションで動かす
            bool no_cost_flg = true;//現在のコストで行動できるかどうか判断用
            for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                if(chara->isOnField()) {//フィールドにいるキャラ
                    if(chara->getActiveSkillId()>0 && chara->getActiveSkillCost()<=dung->player[TurnManage::getPlayerIndex()]->getNowPower()) {//スキルを発動できるコストがあるか
                        no_cost_flg = false;
                        break;
                    }
                }
                if(chara->getHandIndex()>=0) {//手札のキャラ
                    if(chara->getPlaceCost()<=dung->player[TurnManage::getPlayerIndex()]->getNowPower()) {//配置できるか
                        no_cost_flg = false;
                        break;
                    }
                }
            }
            
            //リーダーのスキルを使用するコストがあるか
            if(CharaList::getLeader(TurnManage::getPlayerIndex())->getActiveSkillId()>0 && CharaList::getLeader(TurnManage::getPlayerIndex())->getActiveSkillCost()<=dung->player[TurnManage::getPlayerIndex()]->getNowPower()) no_cost_flg = false;
            //フレンドの配置できるコストがあるか
            if(CharaList::getFriend(TurnManage::getPlayerIndex()) != nullptr) {
                if(CharaList::getFriend(TurnManage::getPlayerIndex())->getActiveSkillId()>0 && CharaList::getFriend(TurnManage::getPlayerIndex())->getActiveSkillCost()<=dung->player[TurnManage::getPlayerIndex()]->getNowPower()) no_cost_flg = false;
            }
            
            if(no_cost_flg){
                changeAttackButton("attack_encourage");//攻撃ボタンのアクション起動
            }else{
                //攻撃ボタンの色合いを元に戻すアニメーション
                TintTo* tin = TintTo::create(0.5, 255, 255, 255);
                attack_img->runAction(tin);

            }
        }else{
            attack_img->setTexture("init/Dungeon/design/battle_bt_defense.png");
            attack_img->setScale(ratio);
            attack_img->setPosition(convertFromDisplaySize(Point(576, 267), 0.5, 0.5));
            attack_img->setColor(old_color);
            TintTo* tin = TintTo::create(0.2, 127, 127, 127);
            attack_img->runAction(tin);
            
        }
        
    }else if(state == "attack_cant"){
        if(dung->isMyTurn()){
            //攻撃ボタンの色を暗くする
            attack_img->stopAllActions();
            attack_img->runAction(ScaleTo::create(0.1f, 1.0f * ratio));
            attack_img->setColor(Color3B(127,127,127));
        }
    }else if(state == "wait"){
        
        //攻撃ボタンの色を暗くする
        attack_img->stopAllActions();
        attack_img->runAction(ScaleTo::create(0.1f, 1.0f * ratio));
        attack_img->setColor(Color3B(127,127,127));
        
    }else if(state == "defense"){
        //攻撃ボタンの色を暗くする
        attack_img->stopAllActions();
        attack_img->runAction(ScaleTo::create(0.1f, 1.0f * ratio));
        attack_img->setColor(Color3B(127,127,127));
        
    }if(state == "attack_encourage"){//アタックボタン拡大縮小アニメーション
        
        //現在かかっているアクションを全て消す
        attack_img->stopAllActions();
        //攻撃ボタンの色合いを元に戻すアニメーション
        TintTo* tin = TintTo::create(0.5, 255, 255, 255);
        attack_img->runAction(tin);
        //拡大・縮小
        ScaleTo* big = ScaleTo::create(0.6f, 1.1f * ratio);
        auto ease1 = EaseSineInOut::create(big);
        //auto spa1 = Spawn::create(ease1,TintTo::create(0.6, 200, 200, 200), NULL);
        ScaleTo* nomal = ScaleTo::create(0.6f, 1.0f * ratio);
        auto ease2 = EaseSineInOut::create(nomal);
        // auto spa2 = Spawn::create(ease2,TintTo::create(0.6, 255, 255, 255), NULL);
        
        Sequence* seq = Sequence::create(ease1, ease2, nullptr);
        RepeatForever* repeat_forever = RepeatForever::create(seq);
        attack_img->runAction(repeat_forever);
    }
    
}

void UILayer::showStamp(int stamp_no ,int index){
    if(SaveData::loadInt("stamp_mute") == 1) return; //ミュートの時

    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    if(dung->getChildByTag(2525)!=nullptr){
        dung->removeChildByTag(2525);
    }
    
    Sprite *stamp = Sprite::create("download/Dungeon/emotion" + to_string(stamp_no) +".png");
    stamp->setScale(ratio * 0.5f);
    if(index == TurnManage::getPlayerIndex()){
        stamp->setPosition(convertFromDisplaySize(Point(250, 264+70), 0.5, 0.5));
    }else{
        stamp->setPosition(convertFromDisplaySize(Point(250, 701+70), 0.5, 0.5));
    }
    dung->damageLayer->addChild(stamp, 100, 2525);

    auto fadein = FadeIn::create(0.1f);
    auto scale = ScaleTo::create(0.1f, ratio*1.15f);
    auto ease1 = EaseBackOut::create(scale);
    auto scale2 = ScaleTo::create(0.15f, ratio);
    auto ease2 = EaseSineInOut::create(scale2);
    
    auto delay = DelayTime::create(2.5f);
    auto fadeout = FadeOut::create(1.2f);
    stamp->runAction(Sequence::create(fadein,ease1,ease2,delay,fadeout,RemoveSelf::create(), NULL));
    
}

void UILayer::updateBossEventRestTime(int boss_event_rest_time)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    if(dung->isBossEventBattle() && boss_event_rest_time<86401) {
        int x = 320;
        int y = 483;
        int z = 7;
        
        string text = "逃走まで";
        int rest_time_ = boss_event_rest_time;
        if(rest_time_>3600) {
            text += to_string(rest_time_ / 3600) + "時間";
            rest_time_ = rest_time_ % 3600;
        }
        if(rest_time_>60) {
            text += to_string(rest_time_ / 60) + "分";
            rest_time_ = rest_time_ % 60;
        }
        else {
            text += to_string(rest_time_) + "秒";
        }
        
        if(this->getChildByTag(REST_TURN_IMG_TAG)==nullptr) {
            auto img_rest_turn = Sprite::create("init/Map/bk_time_left.png");
            img_rest_turn->setPosition(convertFromDisplaySize(Point(x,y), 0.5, 0.5));
            img_rest_turn->setScale(ratio);
            this->addChild(img_rest_turn, z, REST_TURN_IMG_TAG);
        }
        
        this->removeChildByTag(REST_TURN_LABEL_TAG);
        
        //ラベル2
        auto label_free2 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 22*ratio);
        label_free2->setHorizontalAlignment(TextHAlignment::CENTER);
        label_free2->setPosition(convertFromDisplaySize(Point(x,y), 0.5, 0.5));
        label_free2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        this->addChild(label_free2, z, REST_TURN_LABEL_TAG);
    }
}

bool UILayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    if(dung->replay_flg==true) return true;
    
    Rect rect = attack_img->boundingBox();
    if(rect.containsPoint(touch->getLocation()) && dung->data_exec_flg==false && dung->turn_change_wait == false) {
        //攻撃ボタンがクリックされたら
        
        auto scale = ScaleTo::create(0.1f,1.1f*ratio);
        auto ease = EaseBackOut::create(scale);
        attack_img->runAction(ease);
        
        attackButtonClicked = true;
    }
    
    if(menu_img->boundingBox().containsPoint(touch->getLocation())) {//menuボタンがクリックされたら
        auto scale = ScaleTo::create(0.1f,1.1f*ratio);
        auto ease = EaseBackOut::create(scale);
        menu_img->runAction(ease);
        
        menuButtonClicked = true;
    }

    if(stamp_cant_time <= 0.0f){
        if(dung->getBattleMode() == "vs" && stamp_img->boundingBox().containsPoint(touch->getLocation())) {//stampボタンがクリックされたら
            auto scale = ScaleTo::create(0.1f,1.1f*ratio);
            auto ease = EaseBackOut::create(scale);
            stamp_img->runAction(ease);
            
            stampButtonClicked = true;
        }
    }
    
    
 
    Rect rect3;
    for(int ii=0;ii<2;ii++){//パッシブ・バッファのアイコンがクリックされたら
        //CCLOG("パッシブアイコン数:%d",passiveBufferIcon[ii].size());
        //CCLOG("パッシブ数:%d",(int)dung->player[ii]->whole_status_passive.size());
        for(int i=0;i<passiveBufferIcon[ii].size();i++){
            rect3 = passiveBufferIcon[ii].at(i).second -> boundingBox();
            if(rect3.containsPoint(touch->getLocation())){
                //resetIconPosition();
                //CCLOG("パッシブアイコンポジション：%f：%f",passiveBufferIcon[ii].at(i).second->getPositionX(),passiveBufferIcon[ii].at(i).second->getPositionY());
                /*for(Chara *chara : CharaList::getAll()){
                    if(chara->getPrimaryId()==static_cast<DungeonScene *>(getParent())->player[ii]->whole_status_passive.at(i)["primary"]){
                        
                        HMSLOG("この全体パッシブはprim:%dが使用したスキルid:%d 「%s」です", chara->getPrimaryId(), chara->passive_skill_id,chara->passive_skill_detail.c_str());
                    }
                }*/
            }
        }
        //CCLOG("アクティブアイコン数:%d",wholeBufferIcon[ii].size());
        //CCLOG("アクティブ数:%d",(int)dung->player[ii]->whole_status_active.size());
        for(int i=0;i<wholeBufferIcon[ii].size();i++){
            rect3 = wholeBufferIcon[ii].at(i).second -> boundingBox();
            if(rect3.containsPoint(touch->getLocation())){
                //CCLOG("アクティブアイコンポジション：%f：%f",wholeBufferIcon[ii].at(i).second->getPositionX(),wholeBufferIcon[ii].at(i).second->getPositionY());
                /*for(Chara *chara : CharaList::getAll()){
                    if(chara->getPrimaryId()==static_cast<DungeonScene *>(getParent())->player[ii]->whole_status_active.at(i)["primary"]){
                        
                        HMSLOG("この全体アクティブはprim:%dが使用したスキルid:%d 「%s」です", chara->getPrimaryId(), chara->active_skill_id,chara->active_skill_detail.c_str());
                    }
                }*/
            }
        }
    }

    return true;
}

void UILayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}


void UILayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    if(dung->replay_flg==true) return;

    //自分のターンの"place_character"で、データ実行中ではない、ターン変更メッセージが来ていない、キャラを掴んだ状態でない
    if(attack_img->boundingBox().containsPoint(touch->getLocation()) && dung->isCharacterPlacable() && dung->data_exec_flg==false && dung->turn_change_wait == false && dung->tapped_chara==nullptr && attack_button_compel_false==false && attackButtonClicked==true) {//攻撃ボタンがクリックされたら
        
        dung->attackButtonClicked();
        
    }
    auto scale = ScaleTo::create(0.1f,1.0f*ratio);
    auto ease = EaseBackOut::create(scale);
    attack_img->runAction(ease);
    
    
    if(menu_img->boundingBox().containsPoint(touch->getLocation()) && menuButtonClicked==true){//menuボタンがクリックされたら
        auto popup_layer = DungeonSubMenuLayer::create();
        dung->addChild(popup_layer, 10000, SUBMENU_LAYER_TAG);
    }
    auto scale2 = ScaleTo::create(0.1f,1.0f*ratio);
    auto ease2 = EaseBackOut::create(scale2);
    menu_img->runAction(ease2);
    
    
    if(dung->getBattleMode() == "vs"){
        if(stamp_cant_time <= 0.0f){
            if(stamp_img->boundingBox().containsPoint(touch->getLocation()) && stampButtonClicked==true){//stampボタンがクリックされたら
                auto stamp_layer = StampLayer::create();
                dung->addChild(stamp_layer, 9999);
                //stamp_cant_time = 2.0f;//X秒間はスタンプ不可
            }
            auto scale3 = ScaleTo::create(0.1f,1.0f*ratio);
            auto ease3 = EaseBackOut::create(scale3);
            stamp_img->runAction(ease3);
        }
    }
    
    attackButtonClicked = false;
    menuButtonClicked = false;
    stampButtonClicked = false;
}

void UILayer::update(float dt){
    stamp_cant_time -= dt;
    if(stamp_cant_time < 0) stamp_cant_time = 0.0f;
}
