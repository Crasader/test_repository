#include "TeamMemberCharacterDetailLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
//#include "UserDefaultUtil.h"
#include "Utility.h"
#include "HMSFontUtil.h"
#include "NormalScene.h"
//#include "HomeCharacterAnimation.h"

#include "BackGroundLayer.h"

USING_NS_CC;

#define kTeamMemberCharacterDetailLayerPriority kCCMenuHandlerPriority-1

bool TeamMemberCharacterDetailLayer::init()
{
    if(!CCLayer::init()) {
        return false;
    }
    
    //タッチ制限
    //this->setTouchPriority(kTeamMemberCharacterDetailLayerPriority);
    
    return true;
}

void TeamMemberCharacterDetailLayer::setData(int user_master_id, int select_party_id, string use_name)
{
    NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent()->getParent();
    
    this->use_name = use_name;
    this->tab_name = "skill";
    
    //ヘッダー&フッター退避//
    //normal_scene->teamHeaderRemove();
    //normal_scene->footerRemove();
    
    ////リスト設定////
    //メンバー、リーダー引き継ぎ
    if(use_name=="member" || use_name=="leader_change") {
        list.import_tsv(SaveData::loadString("team_demand_dat"));
    }
    //志願者
    else if(use_name=="app") {
        list.import_tsv(SaveData::loadString("team_invitation_user_list"));
    }
    //勧誘中
    else if(use_name=="soli") {
        list.import_tsv(SaveData::loadString("team_invitation_search_list"));
    }
    //検索結果
    else if(use_name=="search") {
        list.import_tsv(SaveData::loadString("team_invitation_search_list"));
    }
    //list.print();
    
    //対象ユーザのインデックス検索
    for(int i=0; i<list.size(); i++) {
        if(user_master_id==fromString<int>(list.getValue(i, "user_master_id"))) {
            member_index = i;
            break;
        }
    }
    
    //初期選択タグ
    this->select_party_id = select_party_id;
    
    ////フェードイン風に見せる////
    Sprite *blind = Sprite::create("init/Home/blind.png");
    blind->setPosition(convertFrom640x960(Point(320, 480)));
    blind->setScale(2*getVisibleRatio());
    this->addChild(blind, 999);
    ActionInterval *fade_out = CCFadeOut::create(0.6f);
    blind->runAction(fade_out);
    //////////////////////////
    
    ////基本画像////
    //背景
    //BackGroundLayer *back_layer = BackGroundLayer::create();
    //this->addChild(back_layer, -9999);
    //back_layer->setData();
    
    //背景2
    Sprite *bg = Sprite::create("init/Home/chara_detail/back.png");
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    bg->setScale(getVisibleRatio());
    this->addChild(bg, 0);
    
    //背景3(上のはみ出している部分を隠す)
    Sprite *bg_black = Sprite::create("init/Home/blind.png");
    bg_black->setPosition(convertFromDisplaySize(Point(320, 960), 0.5, 1));
    bg_black->setScale(getVisibleRatio());
    this->addChild(bg_black, -10000);
    
    //戻る
    Sprite *back = Sprite::create("init/Home/Common/bt_return.png");
    back->setPosition(convertFromDisplaySize(Point(52, 930), 0.5, 1));
    back->setScale(getVisibleRatio());
    this->addChild(back, 1, 1);
    
    //キャラ名系
    Sprite *character_name = Sprite::create("init/Home/mypage/mypage_name_bk.png");
    character_name->setPosition(convertFromDisplaySize(Point(320, 280), 0.5, 0));
    character_name->setScale(getVisibleRatio());
    this->addChild(character_name, 1);
    
    //コスト//
    Sprite *cost = Sprite::create("init/Home/chara_detail/cost.png");
    cost->setPosition(convertFromDisplaySize(Point(70, 870), 0.5, 1));
    cost->setScale(getVisibleRatio());
    this->addChild(cost, 1);
    ////////
    
    //タイプ//
    Sprite *type = Sprite::create("init/Home/chara_detail/type.png");
    type->setPosition(convertFromDisplaySize(Point(420, 870), 0.5, 0.5));
    type->setScale(getVisibleRatio());
    this->addChild(type, 1);
    
    Sprite *type_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    type_under_bar->setPosition(convertFromDisplaySize(Point(640, 855), 0.5, 0.5));
    type_under_bar->setScale(getVisibleRatio());
    type_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(type_under_bar, 1);
    ////////
    
    //レベル//
    Sprite *level = Sprite::create("init/Home/chara_detail/level.png");
    level->setPosition(convertFromDisplaySize(Point(420, 820), 0.5, 0.5));
    level->setScale(getVisibleRatio());
    this->addChild(level, 1);
    
    Sprite *level_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    level_under_bar->setPosition(convertFromDisplaySize(Point(640, 805), 0.5, 0.5));
    level_under_bar->setScale(getVisibleRatio());
    level_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(level_under_bar, 1);
    ////////
    
    //次のレベルまで//
    Sprite *next_level = Sprite::create("init/Home/chara_detail/nextlevel.png");
    next_level->setPosition(convertFromDisplaySize(Point(425, 740), 0.5, 0.5));
    next_level->setScale(getVisibleRatio());
    this->addChild(next_level, 1);
    ///////////////
    
    //あと//
    Sprite *ato = Sprite::create("init/Home/chara_detail/ato.png");
    ato->setPosition(convertFromDisplaySize(Point(415, 705), 0.5, 0.5));
    ato->setScale(getVisibleRatio());
    this->addChild(ato, 1);
    ///////
    
    //経験値ゲージ//
    Sprite *experience_flame = Sprite::create("init/Home/chara_detail/gaugecover.png");
    experience_flame->setPosition(convertFromDisplaySize(Point(600, 685), 0.5, 0.5));
    experience_flame->setScale(getVisibleRatio());
    this->addChild(experience_flame, 2);
    //////////////
    
    //HP//
    Sprite *hp = Sprite::create("init/Home/chara_detail/hp.png");
    hp->setPosition(convertFromDisplaySize(Point(435, 560), 0.5, 0.5));
    hp->setScale(getVisibleRatio());
    this->addChild(hp, 1);
    
    Sprite *hp_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    hp_under_bar->setPosition(convertFromDisplaySize(Point(640, 545), 0.5, 0.5));
    hp_under_bar->setScale(getVisibleRatio());
    hp_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(hp_under_bar, 1);
    //////
    
    //攻撃力//
    Sprite *offense = Sprite::create("init/Home/chara_detail/attack.png");
    offense->setPosition(convertFromDisplaySize(Point(435, 510), 0.5, 0.5));
    offense->setScale(getVisibleRatio());
    this->addChild(offense, 1);
    
    Sprite *offense_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    offense_under_bar->setPosition(convertFromDisplaySize(Point(640, 495), 0.5, 0.5));
    offense_under_bar->setScale(getVisibleRatio());
    offense_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(offense_under_bar, 1);
    ////////
    
    //防御力//
    Sprite *defense = Sprite::create("init/Home/chara_detail/defense.png");
    defense->setPosition(convertFromDisplaySize(Point(435, 460), 0.5, 0.5));
    defense->setScale(getVisibleRatio());
    this->addChild(defense, 1);
    
    Sprite *defense_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    defense_under_bar->setPosition(convertFromDisplaySize(Point(640, 445), 0.5, 0.5));
    defense_under_bar->setScale(getVisibleRatio());
    defense_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(defense_under_bar, 1);
    ////////
    
    //SPEED//
    Sprite *speed = Sprite::create("init/Home/chara_detail/speed.png");
    speed->setPosition(convertFromDisplaySize(Point(435, 410), 0.5, 0.5));
    speed->setScale(getVisibleRatio());
    this->addChild(speed, 1);
    
    Sprite *speed_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    speed_under_bar->setPosition(convertFromDisplaySize(Point(640, 395), 0.5, 0.5));
    speed_under_bar->setScale(getVisibleRatio());
    speed_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(speed_under_bar, 1);
    ////////
    
    //MP//
    Sprite *mp = Sprite::create("init/Home/chara_detail/mp.png");
    mp->setPosition(convertFromDisplaySize(Point(435, 360), 0.5, 0.5));
    mp->setScale(getVisibleRatio());
    this->addChild(mp, 1);
    
    Sprite *mp_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    mp_under_bar->setPosition(convertFromDisplaySize(Point(640, 345), 0.5, 0.5));
    mp_under_bar->setScale(getVisibleRatio());
    mp_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(mp_under_bar, 1);
    /////
    
    //BATTLE//
    Sprite *battle = Sprite::create("init/Home/chara_detail/battle.png");
    battle->setPosition(convertFromDisplaySize(Point(435, 230), 0.5, 0));
    battle->setScale(getVisibleRatio());
    this->addChild(battle, 1);
    
    Sprite *battle_under_bar = Sprite::create("init/Home/chara_detail/bar.png");
    battle_under_bar->setPosition(convertFromDisplaySize(Point(640, 215), 0.5, 0));
    battle_under_bar->setScale(getVisibleRatio());
    battle_under_bar->setAnchorPoint(Point(1, 0.5));
    this->addChild(battle_under_bar, 1);
    //////////
    
    //タブ内容//
    Sprite *tab_content = Sprite::create("init/Home/chara_detail/tab_back.png");
    tab_content->setPosition(convertFromDisplaySize(Point(320, 76), 0.5, 0));
    tab_content->setScale(getVisibleRatio());
    this->addChild(tab_content, 1);
    
    //更新
    update();
    
    //タップ許可&タップモード設定(シングルタッチ)
    this->setTouchEnabled(true);
    this->setTouchMode(kCCTouchesOneByOne);
}

void TeamMemberCharacterDetailLayer::update()
{
    //キャラ画像//
    removeChildByTag(100, true);
    string file_name = "download/Home/chara/";
    file_name += list.getValue(member_index, "chara_id_" + toString(select_party_id));
    file_name += ".png";
    Sprite *character = Sprite::create(file_name.c_str());
    int detail_x = fromString<int>(list.getValue(member_index, "detail_x_" + toString(select_party_id)));
    int detail_y = fromString<int>(list.getValue(member_index, "detail_y_" + toString(select_party_id)));
    character->setPosition(convertFromDisplaySize(Point(250+detail_x, 480+detail_y), 0.5, 1));
    character->setScale(1.5f*getVisibleRatio());
    this->addChild(character, -1, 100);
    
    //コスト値//
    removeChildByTag(101, true);
    auto cost_value = HMSFontUtil::createWithTTFBySystemFontArgument(list.getValue(member_index, "cost_" + toString(select_party_id)), FONT_NAME_SYSTEM, 28);
    //HmsFont *cost_value = HmsFont::create();
    //cost_value->setText("init/Home/num_silver/", list.getValue(member_index, "cost_" + toString(select_party_id)), 28*getVisibleRatio(), 1*getVisibleRatio());
    //cost_value->setPosition(convertFromDisplaySize(Point(105, 855), 0.5, 1), Point(0, 0));
    cost_value->setAnchorPoint(Point(0, 0));
    cost_value->setPosition(convertFromDisplaySize(Point(105, 855), 0.5, 1));
    this->addChild(cost_value, 1, 101);
    
    //タイプ値//
    removeChildByTag(102, true);
    string type_value_file = "download/Home/type_name/";
    type_value_file += list.getValue(member_index, "class_id_" + toString(select_party_id));
    type_value_file += "_";
    type_value_file += list.getValue(member_index, "upper_flg_" + toString(select_party_id));
    type_value_file += ".png";
    Sprite *type_value = Sprite::create(type_value_file.c_str());
    type_value->setPosition(convertFrom640x960(Point(640, 870)));
    type_value->setScale(getVisibleRatio());
    type_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(type_value, 1, 102);
    
    //レベル値//
    removeChildByTag(103, true);
    string level_str = list.getValue(member_index, "level_" + toString(select_party_id));
    level_str += "/";
    level_str += list.getValue(member_index, "max_level_" + toString(select_party_id));
    CCLabelTTF *level_value = CCLabelTTF::create(level_str.c_str(), FONT_NAME, 30);
    level_value->setPosition(convertFrom640x960(Point(615, 820)));
    level_value->setScale(getVisibleRatio());
    level_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(level_value, 1, 103);
    
    //次のレベルまでの経験値値//
    removeChildByTag(104, true);
    auto next_level_value = HMSFontUtil::createWithTTFBySystemFontArgument(list.getValue(member_index, "experience_next_" + toString(select_party_id)), FONT_NAME_SYSTEM, 18);
    //HmsFont *next_level_value = HmsFont::create();
    //next_level_value->setText("init/Home/num_gold/", list.getValue(member_index, "experience_next_" + toString(select_party_id)), 18*getVisibleRatio(), 1*getVisibleRatio());
    next_level_value->setAnchorPoint(Point(0, 0.5));
    next_level_value->setPosition(convertFrom640x960(Point(440, 708)));
    this->addChild(next_level_value, 1, 104);
    
    //HP値//
    removeChildByTag(105, true);
    CCLabelTTF *hp_value = CCLabelTTF::create(list.getValue(member_index, "hp_" + toString(select_party_id)).c_str(), FONT_NAME, 30);
    hp_value->setPosition(convertFrom640x960(Point(615, 560)));
    hp_value->setScale(getVisibleRatio());
    hp_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(hp_value, 1, 105);
    
    //攻撃力値//
    removeChildByTag(106, true);
    CCLabelTTF *offense_value = CCLabelTTF::create(list.getValue(member_index, "offense_" + toString(select_party_id)).c_str(), FONT_NAME, 30);
    offense_value->setPosition(convertFrom640x960(Point(615, 510)));
    offense_value->setScale(getVisibleRatio());
    offense_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(offense_value, 1, 106);
    
    //防御力値//
    removeChildByTag(107, true);
    CCLabelTTF *defense_value = CCLabelTTF::create(list.getValue(member_index, "defense_" + toString(select_party_id)).c_str(), FONT_NAME, 30);
    defense_value->setPosition(convertFrom640x960(Point(615, 460)));
    defense_value->setScale(getVisibleRatio());
    defense_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(defense_value, 1, 107);
    
    //SPEED値//
    removeChildByTag(108, true);
    CCLabelTTF *speed_value = CCLabelTTF::create(list.getValue(member_index, "speed_rank_" + toString(select_party_id)).c_str(), FONT_NAME, 30);
    speed_value->setPosition(convertFrom640x960(Point(615, 410)));
    speed_value->setScale(getVisibleRatio());
    speed_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(speed_value, 1, 108);
    
    //MP値//
    removeChildByTag(109, true);
    CCLabelTTF *mp_value = CCLabelTTF::create(list.getValue(member_index, "mp_" + toString(select_party_id)).c_str(), FONT_NAME, 30);
    mp_value->setPosition(convertFrom640x960(Point(615, 360)));
    mp_value->setScale(getVisibleRatio());
    mp_value->setAnchorPoint(Point(1, 0.5));
    this->addChild(mp_value, 1, 109);
    
    //ドット画像
    //removeChildByTag(110, true);
    //HomeCharacterAnimation *dot = HomeCharacterAnimation::create();
    //dot->setPosition(convertFromDisplaySize(Point(560, 192), 0.5, 0));
    //dot->setData(fromString<int>(list.getValue(member_index, "chara_id_" + toString(select_party_id))));
    //this->addChild(dot, 0, 110);
    
    //キャラ名画像//
    removeChildByTag(112, true);
    string character_name_file = "download/Home/chara_name/";
    character_name_file += list.getValue(member_index, "chara_id_" + toString(select_party_id));
    character_name_file += ".png";
    Sprite *character_name = Sprite::create(character_name_file.c_str());
    character_name->setPosition(convertFromDisplaySize(Point(200, 274), 0.5, 0));
    character_name->setScale(getVisibleRatio());
    this->addChild(character_name, 1, 112);
    
    //経験値ゲージ//
    removeChildByTag(113, true);
    Sprite *experience_gauge = Sprite::create("init/Home/chara_detail/gauge.png");
    experience_gauge->setPosition(convertFromDisplaySize(Point(596, 585), 0.5, 0.5));
    experience_gauge->setScale(getVisibleRatio());
    experience_gauge->setScaleY(fromString<float>(list.getValue(member_index, "experience_percent_" + toString(select_party_id)))/100.0f*getVisibleRatio());
    experience_gauge->setAnchorPoint(Point(0.5, 0));
    this->addChild(experience_gauge, 1, 113);
    
    ////////スキルタブ////////
    removeChildByTag(200, true);
    removeChildByTag(201, true);
    removeChildByTag(202, true);
    removeChildByTag(203, true);
    removeChildByTag(204, true);
    removeChildByTag(205, true);
    removeChildByTag(206, true);
    string skill_tab_file = "";
    if(this->tab_name=="skill") {
        skill_tab_file = "init/Home/chara_detail/skill_2.png";
        
        //スキル枠
        Sprite *skill_tab_main = Sprite::create("init/Home/chara_detail/skill_mein.png");
        skill_tab_main->setPosition(convertFromDisplaySize(Point(320, 76), 0.5, 0));
        skill_tab_main->setScale(getVisibleRatio());
        this->addChild(skill_tab_main, 1, 201);
        
        //リーダースキル名
        string leader_skill_name_str = list.getValue(member_index, "leader_skill_name_" + toString(select_party_id));
        CCLabelTTF *leader_skill_name = CCLabelTTF::create(leader_skill_name_str.c_str(), FONT_NAME, 20);
        leader_skill_name->setPosition(convertFromDisplaySize(Point(240, 130), 0.5, 0));
        leader_skill_name->setScale(getVisibleRatio());
        leader_skill_name->setAnchorPoint(Point(0, 0.5));
        this->addChild(leader_skill_name, 1, 202);
        
        //リーダースキル説明
        string leader_skill_detail_str = list.getValue(member_index, "leader_skill_detail_" + toString(select_party_id));
        CCLabelTTF *leader_skill_detail = CCLabelTTF::create(leader_skill_detail_str.c_str(), FONT_NAME, 20);
        leader_skill_detail->setPosition(convertFromDisplaySize(Point(20, 95), 0.5, 0));
        leader_skill_detail->setScale(getVisibleRatio());
        leader_skill_detail->setAnchorPoint(Point(0, 0.5));
        this->addChild(leader_skill_detail, 1, 203);
        
        //スキル名
        string skill_name_str = list.getValue(member_index, "skill_name_" + toString(select_party_id));
        CCLabelTTF *skill_name = CCLabelTTF::create(skill_name_str.c_str(), FONT_NAME, 20);
        skill_name->setPosition(convertFromDisplaySize(Point(240, 55), 0.5, 0));
        skill_name->setScale(getVisibleRatio());
        skill_name->setAnchorPoint(Point(0, 0.5));
        this->addChild(skill_name, 1, 204);
        
        //スキル説明
        string skill_detail_str = list.getValue(member_index, "skill_detail_" + toString(select_party_id));
        CCLabelTTF *skill_detail = CCLabelTTF::create(skill_detail_str.c_str(), FONT_NAME, 20);
        skill_detail->setPosition(convertFromDisplaySize(Point(20, 15), 0.5, 0));
        skill_detail->setScale(getVisibleRatio());
        skill_detail->setAnchorPoint(Point(0, 0.5));
        this->addChild(skill_detail, 1, 205);
        
        //スキルレベル
        string skill_level_str = list.getValue(member_index, "skill_level_" + toString(select_party_id));
        CCLabelTTF *skill_level = CCLabelTTF::create(skill_level_str.c_str(), FONT_NAME, 20);
        skill_level->setPosition(convertFromDisplaySize(Point(620, 52), 0.5, 0));
        skill_level->setScale(getVisibleRatio());
        skill_level->setAnchorPoint(Point(1, 0.5));
        this->addChild(skill_level, 1, 206);
        
    } else {
        skill_tab_file = "init/Home/chara_detail/skill_1.png";
    }
    Sprite *skill_tab = Sprite::create(skill_tab_file.c_str());
    skill_tab->setPosition(convertFromDisplaySize(Point(62, 182), 0.5, 0));
    skill_tab->setScale(getVisibleRatio());
    this->addChild(skill_tab, 1, 200);
    ////////////////////////
    
    ////////装備タブ////////
    removeChildByTag(300, true);
    removeChildByTag(301, true);
    removeChildByTag(302, true);
    removeChildByTag(303, true);
    removeChildByTag(304, true);
    string equip_tab_file = "";
    if(this->tab_name=="equip") {
        equip_tab_file = "init/Home/chara_detail/armor_2.png";
        
        ////武器////
        Sprite *weapon_back = Sprite::create("init/Home/chara_detail/armorbg1.png");
        weapon_back->setPosition(convertFromDisplaySize(Point(76, 76), 0.5, 0));
        weapon_back->setScale(getVisibleRatio());
        this->addChild(weapon_back, 1, 301);
        
        if(fromString<int>(list.getValue(member_index, "weapon_id_" + toString(select_party_id))) > 0) {
            string weapon_file = "download/Home/equip_160/";
            weapon_file += list.getValue(member_index, "weapon_id_" + toString(select_party_id));
            weapon_file += ".png";
            Sprite *weapon = Sprite::create(weapon_file.c_str());
            weapon->setAnchorPoint(Point(0, 0));
            weapon_back->addChild(weapon);
        }
        
        Sprite *weapon_flame = Sprite::create("init/Home/chara_detail/armorwaku1.png");
        weapon_flame->setAnchorPoint(Point(0, 0));
        weapon_back->addChild(weapon_flame);
        ///////////
        
        ////防具////
        Sprite *guard_back = Sprite::create("init/Home/chara_detail/armorbg2.png");
        guard_back->setPosition(convertFromDisplaySize(Point(228, 76), 0.5, 0));
        guard_back->setScale(getVisibleRatio());
        this->addChild(guard_back, 1, 302);
        
        if(fromString<int>(list.getValue(member_index, "guard_id_" + toString(select_party_id))) > 0) {
            string guard_file = "download/Home/equip_160/";
            guard_file += list.getValue(member_index, "guard_id_" + toString(select_party_id));
            guard_file += ".png";
            Sprite *guard = Sprite::create(guard_file.c_str());
            guard->setAnchorPoint(Point(0, 0));
            guard_back->addChild(guard);
        }
        
        Sprite *guard_flame = Sprite::create("init/Home/chara_detail/armorwaku2.png");
        guard_flame->setAnchorPoint(Point(0, 0));
        guard_back->addChild(guard_flame);
        ///////////
        
        ////装飾////
        Sprite *accessory_back = Sprite::create("init/Home/chara_detail/armorbg3.png");
        accessory_back->setPosition(convertFromDisplaySize(Point(380, 76), 0.5, 0));
        accessory_back->setScale(getVisibleRatio());
        this->addChild(accessory_back, 1, 303);
        
        if(fromString<int>(list.getValue(member_index, "accessory_id_" + toString(select_party_id))) > 0) {
            string accessory_file = "download/Home/equip_160/";
            accessory_file += list.getValue(member_index, "accessory_id_" + toString(select_party_id));
            accessory_file += ".png";
            Sprite *accessory = Sprite::create(accessory_file.c_str());
            accessory->setAnchorPoint(Point(0, 0));
            accessory_back->addChild(accessory);
        }
        
        Sprite *accessory_flame = Sprite::create("init/Home/chara_detail/armorwaku3.png");
        accessory_flame->setAnchorPoint(Point(0, 0));
        accessory_back->addChild(accessory_flame);
        ///////////
        
        ////オススメ装備////
        //Sprite *recommend = Sprite::create("init/Home/chara_detail/optimum.png");
        //recommend->setPosition(convertFromDisplaySize(Point(640, 0), 0.5, 0));
        //recommend->setScale(getVisibleRatio());
        //recommend->setAnchorPoint(Point(1, 0));
        //this->addChild(recommend, 1, 304);
        //////////////////
        
    } else {
        equip_tab_file = "init/Home/chara_detail/armor_1.png";
    }
    Sprite *equip_tab = Sprite::create(equip_tab_file.c_str());
    equip_tab->setPosition(convertFromDisplaySize(Point(186, 182), 0.5, 0));
    equip_tab->setScale(getVisibleRatio());
    this->addChild(equip_tab, 1, 300);
    ///////////////////////
    
    ////////キャラ説明タブ////////
    removeChildByTag(400, true);
    removeChildByTag(401, true);
    string description_tab_file = "";
    if(this->tab_name=="description") {
        description_tab_file = "init/Home/chara_detail/chara_2.png";
        
        //キャラ説明文
        string description_str = list.getValue(member_index, "chara_detail_" + toString(select_party_id));
        CCLabelTTF *description = CCLabelTTF::create(description_str.c_str(), FONT_NAME, 20);
        description->setPosition(convertFromDisplaySize(Point(10, 70), 0.5, 0));
        description->setScale(getVisibleRatio());
        description->setAnchorPoint(Point(0, 0.5));
        description->setDimensions(CCSize(620, 140));
        description->setHorizontalAlignment(CCTextAlignment(0));
        this->addChild(description, 1, 401);
        
    } else {
        description_tab_file = "init/Home/chara_detail/chara_1.png";
    }
    Sprite *description_tab = Sprite::create(description_tab_file.c_str());
    description_tab->setPosition(convertFromDisplaySize(Point(310, 182), 0.5, 0));
    description_tab->setScale(getVisibleRatio());
    this->addChild(description_tab, 1, 400);
    ////////////////////////////
}

bool TeamMemberCharacterDetailLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamMemberCharacterDetailLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamMemberCharacterDetailLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップ位置
    CCPoint tapPoint = touch->getLocation();
    
    //スキルタブ
    Sprite *skill_tab = (Sprite*)this->getChildByTag(200);
    CCRect skill_tab_rect = skill_tab->boundingBox();
    if(skill_tab_rect.containsPoint(tapPoint)) {
        //選択タブ変更
        tab_name = "skill";
        //表示情報更新
        update();
        return;
    }
    
    //装備タブ
    Sprite *equip_tab = (Sprite*)this->getChildByTag(300);
    CCRect equip_tab_rect = equip_tab->boundingBox();
    if(equip_tab_rect.containsPoint(tapPoint)) {
        //選択タブ変更
        tab_name = "equip";
        //表示情報更新
        update();
        return;
    }
    
    //キャラ説明タブ
    Sprite *description_tab = (Sprite*)this->getChildByTag(400);
    CCRect description_tab_rect = description_tab->boundingBox();
    if(description_tab_rect.containsPoint(tapPoint)) {
        //選択タブ変更
        tab_name = "description";
        //表示情報更新
        update();
        return;
    }
    
    //終了
    finishFadeOut();
    return;
}

void TeamMemberCharacterDetailLayer::finishFadeOut()
{
    NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent()->getParent();
    
    ////フェードイン風に見せる////
    Sprite *blind = Sprite::create("init/Home/blind.png");
    blind->setPosition(convertFrom640x960(Point(320, 480)));
    blind->setScale(2*getVisibleRatio());
    this->addChild(blind, 999);
    ActionInterval *fade_in = CCFadeIn::create(0.6f);
    blind->runAction(fade_in);
    //////////////////////////
    
    //ヘッダー&フッター復活//
    //normal_scene->teamHeaderRebirth();
    //normal_scene->footerRebirth();
    
    //終了
    //20151019
    //scheduleOnce(schedule_selector(TeamMemberCharacterDetailLayer::finish), 0.6f);
}

void TeamMemberCharacterDetailLayer::finish()
{
    removeFromParentAndCleanup(true);
}