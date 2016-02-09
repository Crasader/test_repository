#include "CharacterDetailSprite.h"

#define CHARA_DIR "download/Chara/all400/"
#define DECK_PARTS_DIR "init/CharaMenu/"

/**
 *
 * このコードはCharacterDetailを流用するために
 * 変なことをしているので流用しないようにして下さい
 *
 **/

enum {
    TYPE_ALL = 0,
    TYPE_OFFENSE,
    TYPE_DEFENSE,
    TYPE_STRATEGIST,
    TYPE_GENERAL,
};

CharacterDetailSprite* CharacterDetailSprite::create()
{
    CharacterDetailSprite *sprite = new CharacterDetailSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharacterDetailSprite::init()
{
    HMSLOG("CharacterDetailSprite::init()");
    
    ratio = getVisibleRatio();
    
    _base_node = Sprite::create() ;
    
    _base_node->setScale(0) ;
    _base_node->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 1)) ;
    _base_node->setAnchorPoint(Point(0.5f, 0.5f)) ;
    addChild(_base_node, 2);
    
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(_swallowsTouches);
    
    listener->onTouchBegan = [](Touch *touch, Event *event)->bool{return true;} ;
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _base_node);
    _touchListener = listener;
    
    return true;
}

int CharacterDetailSprite::getCharalistIndexByCharaListId(int chara_list_id){
    int result = 0 ;
    for (int i = 0; i< character_list.size(); i++) {
        if(character_list[i]["chara_list_id"] == toString<int>(chara_list_id)){
            result = i ;
        }
    }
    return result ;
}

void CharacterDetailSprite::setupImage(int chara_list_id){
    
    auto normal_scene = (NormalScene *)this->getParent()->getParent()->getParent();
    
    if (!normal_scene) {
        return;
    }
    
    character_list = normal_scene->character_list;
    _active_skill_list = normal_scene->active_skill_list;
    _passive_skill_list = normal_scene->passive_skill_list;
    
    _character_list_index = getCharalistIndexByCharaListId(chara_list_id) ;
    
    //カードデータの読み込み
    getLockData(fromString<int>(character_list[_character_list_index]["chara_list_id"]));
    
    //
    auto spCardDetail = Sprite::create("init/CharaMenu/popup_card_detail.png");
    spCardDetail->setScale(ratio);
    spCardDetail->setAnchorPoint(Point(0.5,1));
    spCardDetail->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,743-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(spCardDetail,1);
    
//    string id = character_list[p]["character_list_index"];
//    
//    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(id, FONT_NAME, 100);
//    label->setScale(ratio);
//    label->setPosition(Point(320-DISPLAY_WIDTH/2, 480-DISPLAY_HEIGHT/2));
//    _base_node->addChild(label, 1);
    
    //カード画像の表示
    auto card = Sprite::create(CHARA_DIR+character_list[_character_list_index]["chara_id"]+".png");
    if (!card) {
        card = Sprite::create("download/Chara/parts100/card_bk1_reverse.png");
        card->setScale(2.28 * ratio);
    }else {
        card->setScale(0.57 * ratio);
    }
    //auto card = Sprite::create(CHARA_DIR"1003.png");
    //card->setScale(0.57 * ratio);
    card->setPosition(convertFromDisplaySize(Point(38-DISPLAY_WIDTH/2,485-DISPLAY_HEIGHT/2), 0, 0));
    card->setAnchorPoint(Vec2(0,0));
    _base_node->addChild(card, 2);
    
    
    //lock画像の表示
    auto lock_not_push = Sprite::create("init/CharaMenu/bt_favorite.png");
    auto lock_push = Sprite::create("init/CharaMenu/bt_favorite.png");
    lock_push->setColor(Color3B(180, 180, 180));
    auto lock_btn = MenuItemSprite::create(lock_not_push, lock_push, CC_CALLBACK_1(CharacterDetailSprite::toggleLockData, this));
    lock_btn->setAnchorPoint(Vec2(0,0));
    lock_btn->setScale(ratio);
    auto lock_menu = Menu::create(lock_btn, nullptr);
    lock_menu->setPosition(convertFromDisplaySize(Point(500-DISPLAY_WIDTH/2,728-DISPLAY_HEIGHT/2), 0, 0));
    lock_menu->setOpacity(0.0f);
    _base_node->addChild(lock_menu, 3, TAG_LOCK_MENU);
    
    //lock画像(灰色画像)の表示
    auto lock_not_push_gray = Sprite::create("init/CharaMenu/bt_favorite_gray.png");
    auto lock_push_gray = Sprite::create("init/CharaMenu/bt_favorite_gray.png");
    lock_push_gray->setColor(Color3B(180, 180, 180));
    auto lock_btn_gray = MenuItemSprite::create(lock_not_push_gray, lock_push_gray, CC_CALLBACK_1(CharacterDetailSprite::toggleLockData, this));
    lock_btn_gray->setAnchorPoint(Vec2(0,0));
    lock_btn_gray->setScale(ratio);
    auto lock_menu_gray = Menu::create(lock_btn_gray, nullptr);
    lock_menu_gray->setPosition(convertFromDisplaySize(Point(500-DISPLAY_WIDTH/2,728-DISPLAY_HEIGHT/2), 0, 0));
    lock_menu_gray->setOpacity(0.0f);
    _base_node->addChild(lock_menu_gray, 3, TAG_LOCK_MENU_GRAY);
    
    //lock画像をlock状況に応じて切り替え
    string lock_fileName = "";
    if (getLockData(fromString<int>(character_list[_character_list_index]["chara_list_id"]))) {
        lock_menu->setOpacity(255) ;
    }else{
        lock_menu_gray->setOpacity(255) ;
    }
    
    //lock->setTexture("init/Home/Mypage/bt_mypage_menu.png");
    
    /*
    string strCharaId = character_list[_character_list_index]["chara_no"];
    auto charaIdLabel = HMSFontUtil::createWithTTFBySystemFontArgument("No."+strCharaId,FONT_NAME_SYSTEM,26);
    charaIdLabel->setScale(ratio);
    charaIdLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    //charaIdLabel->setAnchorPoint(Point(0,1));
     charaIdLabel->enableOutline(Color4B::BLACK,1);
    charaIdLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    charaIdLabel->setPosition(convertFromDisplaySize(Point(380-DISPLAY_WIDTH/2, 785-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(charaIdLabel,2);
    */

    string strName = character_list[_character_list_index]["second_name"];
    strName += " "+character_list[_character_list_index]["name"];
    auto nameLabel = HMSFontUtil::createWithTTFBySystemFontArgument(strName,FONT_NAME_SYSTEM,25);//35
    nameLabel->setScale(ratio);
    //nameLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    nameLabel->setAnchorPoint(Point(0.5,1));
    nameLabel->enableOutline(Color4B::BLACK,1);
    nameLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    nameLabel->setPosition(convertFromDisplaySize(Point(437-DISPLAY_WIDTH/2, 704-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(nameLabel,2);
    
    //キャラクターパラメータ
    int charaType = fromString<int>(character_list[_character_list_index]["chara_type"]);
    string paramVal = "";
    string paramName = "戦闘力";
    string costVal = character_list[_character_list_index]["need_cost"];
    string costName = "配置士気";
    switch (charaType) {
        case TYPE_STRATEGIST:
            paramVal = character_list[_character_list_index]["turn_count"];
            paramName = "継続ターン";
            break;
            
        case TYPE_GENERAL:
            paramVal = character_list[_character_list_index]["hp"];
            paramName = "体力";
            costVal = character_list[_character_list_index]["add_cost"];
            costName = "上昇士気";
            break;
            
        default:
            paramVal = character_list[_character_list_index]["power"];
            break;
    }
    
    auto laPowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(paramName,FONT_NAME_SYSTEM,18);
    laPowerLabel->setScale(ratio);
    //laPowerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laPowerLabel->setAnchorPoint(Point(0.5,1));
    laPowerLabel->enableOutline(Color4B::BLACK,1);
    laPowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laPowerLabel->setPosition(convertFromDisplaySize(Point(350-DISPLAY_WIDTH/2, 652-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(laPowerLabel,2);
    
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(costName,FONT_NAME_SYSTEM,18);
    laCostLabel->setScale(ratio);
    //laCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laCostLabel->setAnchorPoint(Point(0.5,1));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(convertFromDisplaySize(Point(520-DISPLAY_WIDTH/2, 652-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(laCostLabel,2);
    
    auto PowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(paramVal,FONT_NAME_SYSTEM,36);
    PowerLabel->setScale(ratio);
    //PowerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    PowerLabel->setAnchorPoint(Point(0.5,1));
    PowerLabel->enableOutline(Color4B::BLACK,1);
    PowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    PowerLabel->setPosition(convertFromDisplaySize(Point(350-DISPLAY_WIDTH/2, 625-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(PowerLabel,2);
    
    auto CostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(costVal,FONT_NAME_SYSTEM,36);
    CostLabel->setScale(ratio);
    //CostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    CostLabel->setAnchorPoint(Point(0.5,1));
    CostLabel->enableOutline(Color4B::BLACK,1);
    CostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    CostLabel->setPosition(convertFromDisplaySize(Point(520-DISPLAY_WIDTH/2, 625-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(CostLabel,2);
    
    auto charaCommentLabel = HMSFontUtil::createWithTTFBySystemFontArgument(character_list[_character_list_index]["chara_comment"],FONT_NAME_SYSTEM,20);
    //auto charaCommentLabel = HMSFontUtil::createWithTTFBySystemFontArgument("ああああああああああいいいいいいいいいいううううううううううええええええええええおおおおおおおおおおかかかかかかかかかかきききききききききき",FONT_NAME_SYSTEM,18);
    charaCommentLabel->setScale(ratio);
    charaCommentLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    charaCommentLabel->setVerticalAlignment(TextVAlignment::TOP);
    charaCommentLabel->setDimensions(320, 250);
    charaCommentLabel->setAnchorPoint(Point(0.5,1));
    charaCommentLabel->enableOutline(Color4B::BLACK,1);
    charaCommentLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    charaCommentLabel->setPosition(convertFromDisplaySize(Point(438-DISPLAY_WIDTH/2,575-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(charaCommentLabel,2);
    
    
    auto sprSkillBg = Sprite::create("init/Home/Friend/popup_skill2.png");
    sprSkillBg->setScale(ratio);
    sprSkillBg->setAnchorPoint(Point(0.5,1));
    sprSkillBg->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,448-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(sprSkillBg,1);
    
    
    auto sprSkillActBg = Sprite::create("init/Home/Friend/popup_skill_a.png");
    sprSkillActBg->setScale(ratio);
    sprSkillActBg->setAnchorPoint(Point(0.5,1));
    sprSkillActBg->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,435-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(sprSkillActBg,1);
    
    
    auto sprSkillPasBg = Sprite::create("init/Home/Friend/popup_skill_p.png");
    sprSkillPasBg->setScale(ratio);
    sprSkillPasBg->setAnchorPoint(Point(0.5,1));
    sprSkillPasBg->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,315-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(sprSkillPasBg,1);
    
    //カードの各スキルイDをとっておく
    int activeSkillId = fromString<int>(character_list[_character_list_index]["active_skill_id"]);
    int passiveSkillId = fromString<int>(character_list[_character_list_index]["passive_skill_id"]);
    
    //スキル情報の取得
    string activeSkillName = "無し";
    string activeSkillDetail = "";
    string activeSkillCost = "";
    string activeSkillLabel = "";
    for (int i = 0; i< _active_skill_list.size(); i++) {
        if (fromString<int>(_active_skill_list[i]["active_skill_id"]) == activeSkillId) {
            activeSkillName = _active_skill_list[i]["name"];
            activeSkillDetail = _active_skill_list[i]["detail"];
            activeSkillCost = _active_skill_list[i]["need_cost"];
            activeSkillLabel = "発動士気";
        }
    }
    
    //スキル情報の取得
    string passiveSkillName = "無し";
    string passiveSkillDetail = "";
    for (int i = 0; i< _passive_skill_list.size(); i++) {
        if (fromString<int>(_passive_skill_list[i]["passive_skill_id"]) == passiveSkillId) {
            passiveSkillName = _passive_skill_list[i]["name"];
            passiveSkillDetail = _passive_skill_list[i]["detail"];
        }
    }
    
    auto active = HMSFontUtil::createWithTTFBySystemFontArgument(activeSkillName, FONT_NAME_SYSTEM, 24);
    active->setScale(ratio);
    active->setColor(Color3B(255,153,0));
    active->setHorizontalAlignment(TextHAlignment::LEFT);
    active->setAnchorPoint(Point(0, 1));
    active->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    active->enableOutline(Color4B(0, 0, 0, 255), 1);
    active->setPosition(convertFromDisplaySize(Point(230-DISPLAY_WIDTH/2,427-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(active,3);
    
    auto activeText = HMSFontUtil::createWithTTFBySystemFontArgument(activeSkillDetail, FONT_NAME_SYSTEM, 22);
    activeText->setScale(ratio);
    activeText->setDimensions(540, 65);
    activeText->setHorizontalAlignment(TextHAlignment::LEFT);
    activeText->setVerticalAlignment(TextVAlignment::TOP);
    activeText->setAnchorPoint(Point(0.5, 1));
    activeText->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    activeText->enableOutline(Color4B(0, 0, 0, 255), 1);
    activeText->setPosition(convertFromDisplaySize(Point(325-DISPLAY_WIDTH/2,388-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(activeText,3);
    
    auto passive = HMSFontUtil::createWithTTFBySystemFontArgument(passiveSkillName, FONT_NAME_SYSTEM, 24);
    passive->setScale(ratio);
    passive->setColor(Color3B(255,204,0));
    passive->setHorizontalAlignment(TextHAlignment::LEFT);
    passive->setAnchorPoint(Point(0, 1));
    passive->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    passive->enableOutline(Color4B(0, 0, 0, 255), 1);
    passive->setPosition(convertFromDisplaySize(Point(230-DISPLAY_WIDTH/2,307-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(passive,3);
    
    auto passiveText = HMSFontUtil::createWithTTFBySystemFontArgument(passiveSkillDetail, FONT_NAME_SYSTEM, 22);
    passiveText->setScale(ratio);
    passiveText->setDimensions(540, 65);
    passiveText->setHorizontalAlignment(TextHAlignment::LEFT);
    passiveText->setVerticalAlignment(TextVAlignment::TOP);
    passiveText->setAnchorPoint(Point(0.5, 1));
    passiveText->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    passiveText->enableOutline(Color4B(0, 0, 0, 255), 1);
    passiveText->setPosition(convertFromDisplaySize(Point(325-DISPLAY_WIDTH/2,268-DISPLAY_HEIGHT/2), 0 ,0));
    _base_node->addChild(passiveText,3);
    
    auto laSkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(activeSkillLabel, FONT_NAME_SYSTEM, 18);
    laSkillCostLabel->setScale(ratio);
    laSkillCostLabel->setColor(Color3B(255,153,0));
    laSkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laSkillCostLabel->setAnchorPoint(Point(0.5, 1));
    laSkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laSkillCostLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    laSkillCostLabel->setPosition(convertFromDisplaySize(Point(515-DISPLAY_WIDTH/2,424-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(laSkillCostLabel,3);
    
    auto SkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(activeSkillCost, FONT_NAME_SYSTEM, 26);
    SkillCostLabel->setScale(ratio);
    SkillCostLabel->setColor(Color3B(255,153,0));
    SkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    SkillCostLabel->setAnchorPoint(Point(0.5, 1));
    SkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    SkillCostLabel->setPosition(convertFromDisplaySize(Point(575-DISPLAY_WIDTH/2,430-DISPLAY_HEIGHT/2), 0, 0));
    _base_node->addChild(SkillCostLabel,3);
    
    
    //戻るボタン
    auto close_off = Sprite::create("init/Home/Common/bt1.png");
    auto close_label_off = HMSFontUtil::createWithTTFBySystemFontArgument("閉じる", FONT_NAME_SYSTEM, 40);
    close_label_off->setPosition(close_off->getContentSize()/2) ;
    close_off->addChild(close_label_off) ;
    auto close_on = Sprite::create("init/Home/Common/bt1.png");
    auto close_label_on = HMSFontUtil::createWithTTFBySystemFontArgument("閉じる", FONT_NAME_SYSTEM, 40);
    close_label_on->setPosition(close_on->getContentSize()/2) ;
    close_on->addChild(close_label_on) ;
    close_on->setColor(Color3B(180, 180, 180));
    auto back_btn = MenuItemSprite::create(close_off, close_on, CC_CALLBACK_1(CharacterDetailSprite::deleteAction, this));
    back_btn->setScale(ratio);
    auto menu = Menu::create(back_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,115-DISPLAY_HEIGHT/2.1), 0, 0));
    _base_node->addChild(menu, 3);
    
    //HPの表示
//    string hp = character_list[p]["hp"];
//    
//    auto hplabel = HMSFontUtil::createWithTTFBySystemFontArgument("HP : "+hp, FONT_NAME, 30);
//    hplabel->setScale(ratio);
//    hplabel->setPosition(convertFromDisplaySize(Point(440-DISPLAY_WIDTH/2, 480-DISPLAY_HEIGHT/2), 0, 0));
//    _base_node->addChild(hplabel, 2);

    
    
    //お知らせリストを表示
    //    HomeInfoListLayer *layer = HomeInfoListLayer::create();
    //    _base_node->addChild(layer);
    //    layer->setup();
    
//    ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/fire.plist");
//    particle->setPosition(convertFromDisplaySize(Point(320-DISPLAY_WIDTH/2,0-DISPLAY_HEIGHT/2), 0, 0));
//    //particle->setSourcePosition(convertFromDisplaySize(Point(0,0), 0.5, 0));
//    particle->setAnchorPoint(Point(0,0));
//    particle->setScale(ratio * 2);
//    //particle->setAutoRemoveOnFinish(true);
//    _base_node->addChild(particle, 0);
    
}


bool CharacterDetailSprite::getLockData(int chara_list_id)
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
    
    ///HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
    
    return favorite_flg;
}

void CharacterDetailSprite::setLockData(int chara_list_id)
{
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //追加
    if(favorite_character_list!="") favorite_character_list += "/";
    favorite_character_list += toString(chara_list_id);
    
    SaveData::saveString("favorite_character_list", favorite_character_list);
    
}

void CharacterDetailSprite::removeLockData(int chara_list_id)
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

void CharacterDetailSprite::popupAction()
{
    //影をかける
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setTag(CLOSE_BUTTUN_TAG) ;
    this->addChild(shadow, 1);
    shadow->setOpacity(0) ;
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5f, 0.5f)) ;
    shadow->runAction(FadeIn::create(0.2f)) ;
    
    auto scale = ScaleTo::create(0.2f, 1.0);
    _base_node->runAction(scale);
}

void CharacterDetailSprite::deleteAction(Ref* sender)
{
    //影を削除
    Sprite *shadow = (Sprite*)this->getChildByTag(CLOSE_BUTTUN_TAG);
    if(shadow!=nullptr){
        shadow->runAction(Sequence::create(FadeOut::create(0.2f), RemoveSelf::create(), nullptr)) ;
    }
    
    auto scale = ScaleTo::create(0.2f, 0);
    _base_node->runAction(Sequence::create(scale, RemoveSelf::create(), nullptr));
}

int CharacterDetailSprite::touchButtun(Point touchPoint)
{
    
    auto child = this->getChildren();
    
    //タップした座標から、タップしたカードのタグを探して返す。なかった場合は−１を返す。
    for (auto node : child) {
        
        if (node->getTag() > 10) {
            auto btn = (Sprite*)node;
            
            auto btnPoint = btn->convertToWorldSpace(_base_node->getPosition());
            auto btnSize = btn->getContentSize();
            
            //btnPoint = btn->convertToWorldSpace(Point::ZERO);
            //HMSLOG("btn x %f btn y %f",btnPoint.x,btnPoint.y);
            auto btnRect = Rect(btnPoint.x,
                                btnPoint.y,
                                btnSize.width * ratio,
                                btnSize.height * ratio);
            if (btnRect.containsPoint(touchPoint)) {
                int tag = node->getTag();
                return tag;
            }
        }
    }
    return -1;
}

/*
void CharacterDetailSprite::showNew(cocos2d::Sprite *card)
{
    //カードにNEWを表示する
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile("init/CharaMenu/new.plist","init/CharaMenu/new.png");
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(ratio * 2);
    sprite->setPosition(convertFromDisplaySize(Point(200-DISPLAY_WIDTH/2,360-DISPLAY_HEIGHT/2), 0, 0));
    card->addChild(sprite, 10000);
    
    //永続的に発生させる
    auto repeat = RepeatForever::create(animate);
    
    sprite->runAction(repeat);
}

void CharacterDetailSprite::showParam(cocos2d::Sprite *card, string paramVal, string sortParam)
{
    
    if (sortParam == "rare") {
        switch (fromString<int>(paramVal)) {
            case 1:
                paramVal = "N";
                break;
            case 2:
                paramVal = "R";
                break;
            case 3:
                paramVal = "SR";
                break;
            case 4:
                paramVal = "UR";
                break;
            case 5:
                paramVal = "LE";
                break;
            default:
                break;
        }
    }
    
    auto param = HMSFontUtil::createWithTTFBySystemFontArgument(paramVal, FONT_NAME_SYSTEM, 30);
    param->setScale(4);
    //param->setHorizontalAlignment(TextHAlignment::RIGHT);
    //param->setTag(PARAM_TAG);
    param->setAnchorPoint(Point(0.5,0));
//enableOutline(Color4B::BLACK,1);
    param->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    param->setPosition(convertFromDisplaySize(Point(200-DISPLAY_WIDTH/2,-50-DISPLAY_HEIGHT/2), 0, 0));
    card->addChild(param, 3);
    
}

void CharacterDetailSprite::showLock(cocos2d::Sprite *card)
{
    //保護情報の表示
    if (getLockData(card->getTag() - CHARACTER_TAG)) {
        auto lock = Sprite::create(DECK_PARTS_DIR"bt_favorite.png");
        lock->setScale(1.2);
        lock->setAnchorPoint(Point(1,0));
        lock->setPosition(convertFromDisplaySize(Point(420-DISPLAY_WIDTH/2,-20-DISPLAY_HEIGHT/2), 0, 0));
        card->addChild(lock, 2);
        
    }
}

 */


//ロックデータのボタンの色と保存状態を切り替える
void CharacterDetailSprite::toggleLockData(Ref* sender)
{
    Menu *lock_menu = (Menu*)_base_node->getChildByTag(TAG_LOCK_MENU) ;
    Menu *lock_menu_gray = (Menu*)_base_node->getChildByTag(TAG_LOCK_MENU_GRAY) ;
    
    //lock画像をlock状況に応じて切り替え
    string lock_fileName = "";
    if (getLockData(fromString<int>(character_list[_character_list_index]["chara_list_id"]))) {
        lock_menu->setOpacity(0) ;
        lock_menu_gray->setOpacity(255) ;
        removeLockData(fromString<int>(character_list[_character_list_index]["chara_list_id"]));
        HMSLOG("LOCK OFF");
    }else{
        lock_menu->setOpacity(255) ;
        lock_menu_gray->setOpacity(0) ;
        setLockData(fromString<int>(character_list[_character_list_index]["chara_list_id"]));
        HMSLOG("LOCK ON");
    }
    
}
