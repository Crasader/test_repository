#include "TeamDetailBase.h"
#include "Utility.h"
#include "HmsGeometricFunction.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include "TeamListLayerBase.h"

TeamDetailBase* TeamDetailBase::create(json11::Json json)
{
    TeamDetailBase *sprite = new TeamDetailBase();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TeamDetailBase::init(json11::Json json)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    this->teamData = json;
    
    return true;
}

void TeamDetailBase::onEnter()
{
    Layer::onEnter();
    
    //タップイベント
    createEventListener();

    //画面作成
    setupData();
}

void TeamDetailBase::setupData()
{
    //詳細画面生成
    createLayout();
    
    //閉じるボタン
    createCloseButton();
    
    //ボタン生成
    createButton();
}

void TeamDetailBase::createLayout()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5)) ;
    this->addChild(shadow);
    
    auto shadow2 = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow2->setScale(ratio);
    shadow2->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5)) ;
    this->addChild(shadow2);
    
    //枠
    frame = Sprite::create("init/Team/no_belong/team/popup_614_234.png");
    frame->setScale(ratio);
    frame->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(frame, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //ページのタイトル画像
    Sprite* _title_bg = Sprite::create("init/Home/Popup/popup_index_s.png");
    _title_bg->setPosition(Point(frame->getContentSize().width / 2, frame->getContentSize().height + 18));
    frame->addChild(_title_bg, Utility::Z_Popup_Action+1);
    
    //ページタイトル
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("チーム情報", FONT_NAME_SYSTEM, 30);
    title_label->setPosition(_title_bg->getContentSize() / 2);
    _title_bg->addChild(title_label);
    
    //名前
    auto team_name = HMSFontUtil::createWithTTFBySystemFontArgument(teamData["name"].string_value(), FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(teamData["name"].string_value(), 24, 180));
    team_name->setPosition(Point(38, 184));
    team_name->setAnchorPoint(Point(0, 0.5));
    frame->addChild(team_name);
    
    //ギルドタイプ//
    string team_type_str = "" ;
    string team_type = teamData["team_type"].string_value();
    if(team_type == "a") team_type_str = "初心者歓迎" ;
    else if(team_type == "b") team_type_str = "中級者限定" ;
    else if(team_type == "c") team_type_str = "上級者求む" ;
    else team_type_str = "----" ;
    
    //プレイタイプ//
    string play_type_str = "" ;
    string play_type = teamData["play_type"].string_value();
    if(play_type == "a") play_type_str = "まったり" ;
    else if(play_type == "b") play_type_str = "ほどほど" ;
    else if(play_type == "c") play_type_str = "がっつり" ;
    else play_type_str = "----" ;
    
    Label *team_and_play_type = HMSFontUtil::createWithTTFBySystemFontArgument(team_type_str+"/"+play_type_str, FONT_NAME_SYSTEM, 23);
    team_and_play_type->setPosition(Point(230, 186));
    team_and_play_type->setHorizontalAlignment(TextHAlignment::LEFT) ;
    team_and_play_type->setVerticalAlignment(TextVAlignment::TOP);
    team_and_play_type->setAnchorPoint(Point(0, 0.5));
    frame->addChild(team_and_play_type);
    
    //リーダーキャラ
    if(teamData["leader_chara_id"].string_value()!="") {
        string leader_chara_file = "download/Home/chara_thum/";
        leader_chara_file += teamData["leader_chara_id"].string_value();
        leader_chara_file += ".png";
        Sprite *thum = Sprite::create(leader_chara_file.c_str());
        thum->setAnchorPoint(Point(0, 1));
        thum->setPosition(Point(2, 168));
        frame->addChild(thum);
    }
    
    //メンバー数値
    string member_num_value_str = teamData["member_count"].string_value();
    member_num_value_str += "/";
    member_num_value_str += teamData["max_member_count"].string_value();
    auto member_num_value = HMSFontUtil::createWithTTFBySystemFontArgument(member_num_value_str, FONT_NAME_SYSTEM, 22);
    member_num_value->setPosition(Point(511, 184));
    member_num_value->setHorizontalAlignment(TextHAlignment::LEFT);
    member_num_value->setAnchorPoint(Point(0, 0.5)) ;
    frame->addChild(member_num_value);
    
    //チームコメント
    string team_comment_str = teamData["comment"].string_value();
    auto team_comment = HMSFontUtil::createWithTTFBySystemFontArgument(team_comment_str, FONT_NAME_SYSTEM, 22);
    team_comment->setHorizontalAlignment(TextHAlignment::LEFT);
    team_comment->setVerticalAlignment(TextVAlignment::TOP);
    team_comment->setPosition(Point(30, 140));
    team_comment->setAnchorPoint(Point(0, 1));
    team_comment->setDimensions(560, 130);
    frame->addChild(team_comment);
}

void TeamDetailBase::createButton()
{
    
}

void TeamDetailBase::buttonTapAction()
{
    
}

void TeamDetailBase::refreshList()
{
    //リストを更新
    ((TeamListLayerBase*)this->getParent())->refreshList();
    
    //画面を削除
    Utility::closePopupAction(this);
//    this->removeFromParent();
}

void TeamDetailBase::buttonTapCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    
}

void TeamDetailBase::createCloseButton()
{
    //閉じるボタン
    MenuItemImage* closeButton = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close.png", [this](Ref* sender){
        //閉じるアクション
        Utility::closePopupAction(this);
//        this->removeFromParent();
        
    });
    Menu* menu = Menu::create(closeButton, nullptr);
    menu->setPosition(Point(frame->getContentSize().width - 96, frame->getContentSize().height + 18));
    frame->addChild(menu, Utility::Z_Popup_Action+3);
}

void TeamDetailBase::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamDetailBase::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamDetailBase::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamDetailBase::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamDetailBase::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool TeamDetailBase::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamDetailBase::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamDetailBase::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamDetailBase::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
