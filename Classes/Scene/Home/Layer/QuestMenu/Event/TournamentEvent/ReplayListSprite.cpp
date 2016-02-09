#include "ReplayListSprite.h"
#include "ReplayScene.h"

ReplayListSprite::ReplayListSprite()
:_battle_list_id()
,_enemy_user_master_id()
,_enemy_chara_id()
,_enemy_user_name()
,_result_flg(false)
{
}

ReplayListSprite* ReplayListSprite::create(json11::Json json, Node* normal_scene)
{
    ReplayListSprite *sprite = new ReplayListSprite();
    if (sprite && sprite->init(json, normal_scene))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ReplayListSprite::init(json11::Json json, Node* normal_scene)
{
    if ( !Sprite::initWithFile("init/Home/Quest/list_replay.png") )
    {
        return false;
    }
    
    //データの設定
    setupData(json);
    
    _normal_scene = normal_scene;
    
    return true;
}

void ReplayListSprite::setupData(json11::Json json)
{
    //データの設定
    setBattleListId(json["battle_list_id"].string_value());
    setEnemyUserMasterId(json["enemy_user_master_id"].string_value());
    setEnemyCharaId(json["enemy_chara_id"].string_value());
    setEnemyUserName(json["enemy_user_name"].string_value());
    setResultFlg(json["result_flg"].bool_value());
}

void ReplayListSprite::onEnter()
{
    Sprite::onEnter();
    
    //対戦相手の画像の表示
    showEnemySprite();
    
    //対戦相手の情報ラベルの表示
    showEnemyLabel();
    
    //勝敗画像の表示
    showResultSprite();
    
    //リプレイボタンの作成
    createReplaySprite();
}

void ReplayListSprite::showEnemySprite()
{
    //対戦相手の画像表示
    if (_enemy_chara_id != "") {
        auto chara_sp = Sprite::create("download/Chara/thumb100/" + _enemy_chara_id + ".png");
        chara_sp->setPosition(Point(70, this->getContentSize().height / 2));
        this->addChild(chara_sp, Z_Enemy_Sprite, T_Enemy_Sprite);
    }
}

void ReplayListSprite::showEnemyLabel()
{
    //対戦相手の情報ラベルの表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("対戦相手", FONT_NAME_SYSTEM, 24);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(148, 88));
    this->addChild(label);
    
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_enemy_user_name, FONT_NAME_SYSTEM, 34);
    name_label->setAnchorPoint(Point(0, 0.5));
    name_label->setPosition(Point(148, 46));
    this->addChild(name_label, Z_Enemy_Name, T_Enemy_Name);
}

void ReplayListSprite::showResultSprite()
{
    //勝敗画像の表示
    string image_path = "init/Home/Quest/replay_win.png";
    if (_result_flg == false) image_path = "init/Home/Quest/replay_lose.png";
    
    auto result_sp = Sprite::create(image_path);
    result_sp->setPosition(Point(410, this->getContentSize().height / 2));
    this->addChild(result_sp, Z_Result_Sprite, T_Result_Sprite);
}

void ReplayListSprite::createReplaySprite()
{
    //リプレイボタンの作成
    auto replay_off = Sprite::create("init/Home/Quest/bt_playback.png");
    auto replay_on  = Sprite::create("init/Home/Quest/bt_playback.png");
    replay_on->setColor(Color3B(180, 180, 180));
    
    auto replay_btn = MenuItemSprite::create(replay_off, replay_on,[this](Ref* sender){
        //ポップアップ表示後、リプレイの再生
        auto func = [this](){
            SaveData::saveString("battle_list_id_for_replay", _battle_list_id);
            SaveData::saveBool("tournament_replay_flg", true);
            
            auto scene = ReplayScene::createScene();
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);

            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
        };
        
        popupConfirmLayer *layer = popupConfirmLayer::create("この対戦のリプレイを見ますか？");
        layer->setFunction(func, nullptr);
        _normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    });
    replay_btn->setTag(T_Replay_MenuItem);
    
    auto menu = Menu::create(replay_btn, nullptr);
    menu->setPosition(Point(548, this->getContentSize().height / 2));
    this->addChild(menu, Z_Replay_Menu, T_Replay_Menu);
}
