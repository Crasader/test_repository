#include "CharaDetailCellSprite.h"

CharaDetailCellSprite* CharaDetailCellSprite::create(int chara_list_id){

    CharaDetailCellSprite *sprite = new CharaDetailCellSprite();

    if (sprite && sprite->init(chara_list_id))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaDetailCellSprite::init(int chara_list_id)
{
    if ( !Sprite::initWithFile("init/Home/Gacha/list_gacha.png") )//画像サイズ130*640
    {
        return false;
    }
    _chara_list_id = chara_list_id;

    return true;
}

void CharaDetailCellSprite::setData()
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent()->getParent()->getParent();
    
//    map<int, map<string, string> > character_list = normal_scene->character_list;
//    map<int, map<string, string> > active_skill_list = normal_scene->active_skill_list;
//    map<int, map<string, string> > passive_skill_list = normal_scene->passive_skill_list;
    
    int target_index = 0 ;
    
    int max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));
    for (int i=0; i< max_chara_count; i++) {
        if(normal_scene->character_list[i]["chara_list_id"] == toString<int>(_chara_list_id)){
            target_index = i;
            break;
        }
    }
    
    chara_id = normal_scene->character_list[target_index]["chara_id"];                 //該当キャラのID
    chara_no = normal_scene->character_list[target_index]["chara_no"];                 //キャラNo
    second_name = normal_scene->character_list[target_index]["second_name"];           //二つ名
    chara_name = normal_scene->character_list[target_index]["name"];                   //キャラ名
    name_kana = normal_scene->character_list[target_index]["name_kana"];               //キャラのフリガナ
    rare = normal_scene->character_list[target_index]["rare"];                         //レア度
    chara_power = normal_scene->character_list[target_index]["power"];                 //該当キャラの戦闘力
    _chara_type = normal_scene->character_list[target_index]["chara_type"];            //キャラタイプ
    need_cost = normal_scene->character_list[target_index]["need_cost"];               //配置コスト
    hp = normal_scene->character_list[target_index]["hp"];                             //体力
    add_cost = normal_scene->character_list[target_index]["add_cost"];                 //上昇士気
    turn_count = normal_scene->character_list[target_index]["turn_count"];             //継続ターン
    chara_comment = normal_scene->character_list[target_index]["chara_comment"];       //キャラコメント
    active_skill_id = normal_scene->character_list[target_index]["active_skill_id"];   //アクティブスキルID
    passive_skill_id = normal_scene->character_list[target_index]["passive_skill_id"]; //パッシブスキルID
    
    HMSLOG("active_skill_id=%s",active_skill_id.c_str());
    for (int i=0; i< max_chara_count; i++) {
        target_index = i ;
        if(normal_scene->active_skill_list[i]["active_skill_id"] == active_skill_id){
            break;
        }
    }
    active_skill_name = normal_scene->active_skill_list[target_index]["name"];              //アクティブスキル名
    active_skill_detail = normal_scene->active_skill_list[target_index]["detail"];          //アクティブスキル詳細
    active_skill_icon_detail = normal_scene->active_skill_list[target_index]["icon_detail"];//アクティブスキルアイコン
    active_skill_need_cost = normal_scene->active_skill_list[target_index]["need_cost"];    //アクティブスキル必要コスト
    HMSLOG("active_skill_name=%s",active_skill_name.c_str());
    HMSLOG("active_skill_detail=%s",active_skill_detail.c_str());
    if(active_skill_id != "0")//アクティブスキルIDが0ではない場合
    {
        skill_name = active_skill_name;
        skill_detail = active_skill_detail;
        skill_icon_detail = active_skill_icon_detail;
    }else{
        for (int i=0; i< max_chara_count; i++) {
            if(normal_scene->passive_skill_list[i]["passive_skill_id"] == passive_skill_id){
                target_index = i ;
                break;
            }
        }
        skill_name = normal_scene->passive_skill_list[target_index]["name"];               //パッシブスキル名
        skill_detail = normal_scene->passive_skill_list[target_index]["detail"];           //パッシブスキル詳細
        skill_icon_detail = normal_scene->passive_skill_list[target_index]["icon_detail"]; //パッシブスキルアイコン
    }
    
}

void CharaDetailCellSprite::setupImage()
{
    //カード画像
    HMSLOG("chara_id=%s",chara_id.c_str());
    auto ch_id = StringUtils::format("download/Chara/thumb400/%s.png", chara_id.c_str());
    auto card = Sprite::create(ch_id.c_str());
    card->setScale(0.25);
    card->setPosition(Point(70,66));
    this->addChild(card, 4);
    
    //タイプ画像
    string i_type[]={"","offense","defense","strategist","general"};
    auto ic_type = StringUtils::format("init/CharaMenu/icon_%s2.png", i_type[fromString<int>(_chara_type)].c_str());
    auto icon_type = Sprite::create(ic_type.c_str());
    icon_type->setPosition(Point(182,17));
    icon_type->setAnchorPoint(Vec2(1,0));
    this->addChild(icon_type, 5);
    
    /*
    //レアリティ画像
    string rar[]={"","n","r","sr","ur","le"};
    auto rarity = StringUtils::format("init/CharaMenu/icon_%s2.png", rar[fromString<int>(rare)].c_str());
    auto rare_icon = Sprite::create(rarity.c_str());
    rare_icon->setPosition(Point(210,70));
    rare_icon->setAnchorPoint(Vec2(1,0));
    this->addChild(rare_icon, 5);
     */
    
    
    //名前画像準備
    auto name_img = StringUtils::format("download/Chara/name/%s.png", chara_id.c_str());
    auto name = Sprite::create(name_img.c_str());
    
    name->setScale(1.0);
    name->setPosition(Vec2(250,70));
    name->setAnchorPoint(Vec2(0,0));
    this->addChild(name, 5);
    
    if(second_name != ""){//キャラの二つ名がある場合
        //二つ名(ラベル)準備
        auto second_label = HMSFontUtil::createWithTTFBySystemFontArgument(second_name.c_str(),FONT_NAME_SYSTEM,40);
        second_label->setScale(1.0);
        
        //二つ名の左端位置の決定方法: 名前画像の左端 + 名前の画像サイズ + 10
        float second_x = name->getPosition().x + name->getContentSize().width + 10 ;
        
        second_label->enableOutline(Color4B(0,0,0,255),3);
        second_label->setPosition(Vec2(second_x,70));
        second_label->setAnchorPoint(Vec2(0,0));
        this->addChild(second_label, 4, CharaDetailCellSprite::Tag::T_Second_Name);
    }
}

void CharaDetailCellSprite::setupLabel()
{
    string lapower_array[]={"","戦闘力:","戦闘力:","継続ターン:","体力:"};//戦闘力ラベル
    string lacost_array[]={"","配置士気:","配置士気:","配置士気:","上昇士気:"};//コストラベル
    string power_array[]={"",chara_power,chara_power,turn_count,hp};//戦闘力
    string cost_array[]={"",need_cost,need_cost,need_cost,add_cost};//コスト
    
    float power_label_right_x = 280 ;
    if(fromString<int>(_chara_type)==3)power_label_right_x=320;
    else if(fromString<int>(_chara_type)==4)power_label_right_x=270;
    
    //戦闘力ラベル
    auto laPowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lapower_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,26);
    laPowerLabel->setAnchorPoint(Point(1,0.5));
    laPowerLabel->enableOutline(Color4B::BLACK,1);
    laPowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laPowerLabel->setPosition(Point(power_label_right_x, 40));
    this->addChild(laPowerLabel,4);
    
    //戦闘力
    auto PowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(power_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,Utility::calcFlexibleFontSize(power_array[fromString<int>(_chara_type)], 40, 100));
    PowerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    PowerLabel->setAnchorPoint(Point(0,0.5));
    PowerLabel->enableOutline(Color4B::BLACK,1);
    PowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    PowerLabel->setPosition(Point(power_label_right_x+5, 40));
    this->addChild(PowerLabel,4);
    //コストラベル
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lacost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,26);
//    laCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laCostLabel->setAnchorPoint(Point(1,0.5));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(Point(530, 40));
    this->addChild(laCostLabel,4);
    //コスト
    auto costLabel = HMSFontUtil::createWithTTFBySystemFontArgument(cost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,40);
    costLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    costLabel->setAnchorPoint(Point(0,0.5));
    costLabel->enableOutline(Color4B::BLACK,1);
    costLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    costLabel->setPosition(Point(535, 40));
    this->addChild(costLabel,4);
    
}

bool CharaDetailCellSprite::getLockData(int chara_list_id)
{
    //カード保護情報を取得
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    HmsVector<string> favorite_character_array = split(favorite_character_list, "/");
    bool favorite_flg = false;
    for(int i=0; i<favorite_character_array.size(); i++) {
        if(fromString<int>(favorite_character_array.at(i))==chara_list_id) {
            favorite_flg = true;
        }
    }
    
    //HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
    
    return favorite_flg;
}

void CharaDetailCellSprite::setLockData(int chara_list_id)
{
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //追加
    if(favorite_character_list!="") favorite_character_list += "/";
    favorite_character_list += toString(chara_list_id);
    
    SaveData::saveString("favorite_character_list", favorite_character_list);
    
}

void CharaDetailCellSprite::removeLockData(int chara_list_id)
{
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //解除
    string new_favorite_character_list = "";
    HmsVector<string> favorite_character_array = split(favorite_character_list, "/");
    for(int i=0; i<favorite_character_array.size(); i++) {
        if(fromString<int>(favorite_character_array.at(i))==chara_list_id) {
            favorite_character_array.at(i) = "";
        } else {
            if(new_favorite_character_list!="") new_favorite_character_list += "/";
            new_favorite_character_list += favorite_character_array.at(i);
        }
    }
    
    SaveData::saveString("favorite_character_list", new_favorite_character_list);
    
    HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
    
}

void CharaDetailCellSprite::touchButtun(Ref *sender)
{
    /*
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    HMSLOG("lock tag %d",tag);
    if (tag == -1) {
        HMSLOG("not");
        //        return;
    }
    
    if (tag == LOCK_BUTTUN_TAG) {
        string fileName = "";
        Sprite * lock = (Sprite*)this->getChildByTag(LOCK_BUTTUN_TAG);
        
        if (getLockData(_chara_list_id)) {
            fileName = "init/CharaMenu/bt_favorite_gray.png";
            removeLockData(_chara_list_id);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/key-tighten1.mp3",false);
            HMSLOG("LOCK OFF");
        }else{
            fileName = "init/CharaMenu/bt_favorite.png";
            setLockData(_chara_list_id);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/key-in1.mp3",false);
            HMSLOG("LOCK ON");
        }
        lock->setTexture(fileName);
    }
     */
}

void CharaDetailCellSprite::deleteAction(Ref *sender)
{
    auto normal_scene = (NormalScene *)this->getParent();
    //影を削除
    Sprite *shadow = (Sprite *)normal_scene->getChildByTag(SHADOW_BACK_LAYER_TAG);
    if(shadow!=nullptr){
        shadow->runAction(Sequence::create(FadeOut::create(0.2f), RemoveSelf::create(), nullptr));
    }
    auto scale = ScaleTo::create(0.2f, 0);
    this->runAction(Sequence::create(scale, RemoveSelf::create(), nullptr));
}

void CharaDetailCellSprite::onEnter()
{
    Sprite::onEnter();

    setData();//情報の取得

    setupImage();//画像の設置

    setupLabel();//文字の設置
}