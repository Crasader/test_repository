#include "TeamDetailSolicitationLayer.h"
#include "HmsGeometricFunction.h"
#include "Utility.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

TeamDetailSolicitationLayer* TeamDetailSolicitationLayer::create(json11::Json json)
{
    TeamDetailSolicitationLayer *sprite = new TeamDetailSolicitationLayer();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void TeamDetailSolicitationLayer::createLayout()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5)) ;
    this->addChild(shadow);
    
    //枠
    frame = Sprite::create("init/Team/management/search/popup_team.png");
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
    team_name->setPosition(Point(38, frame->getContentSize().height - 50));
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

    
    Label *team_type_label = HMSFontUtil::createWithTTFBySystemFontArgument(team_type_str, FONT_NAME_SYSTEM, 23);
    team_type_label->setPosition(Point(290, frame->getContentSize().height - 50));
    team_type_label->setHorizontalAlignment(TextHAlignment::LEFT);
    team_type_label->setVerticalAlignment(TextVAlignment::TOP);
    team_type_label->setAnchorPoint(Point(0.5, 0.5));
    frame->addChild(team_type_label);
    
    Label *play_type_label = HMSFontUtil::createWithTTFBySystemFontArgument(play_type_str, FONT_NAME_SYSTEM, 23);
    play_type_label->setPosition(Point(410, frame->getContentSize().height - 50));
    play_type_label->setHorizontalAlignment(TextHAlignment::LEFT) ;
    play_type_label->setVerticalAlignment(TextVAlignment::TOP);
    play_type_label->setAnchorPoint(Point(0.5, 0.5));
    frame->addChild(play_type_label);
    
    //リーダーキャラ
    if(teamData["leader_chara_id"].string_value()!="") {
        string leader_chara_file = "download/Chara/thumb100/";
        leader_chara_file += teamData["leader_chara_id"].string_value();
        leader_chara_file += ".png";
        Sprite *thum = Sprite::create(leader_chara_file.c_str());
        thum->setAnchorPoint(Point(0, 1));
        thum->setPosition(Point(26, 128));
        frame->addChild(thum);
    }
    
    //メンバー数値
    string member_num_value_str = teamData["member_count"].string_value();
    member_num_value_str += "/";
    member_num_value_str += teamData["max_member_count"].string_value();
    auto member_num_value = HMSFontUtil::createWithTTFBySystemFontArgument(member_num_value_str, FONT_NAME_SYSTEM, 22);
    member_num_value->setPosition(Point(511, frame->getContentSize().height - 50));
    member_num_value->setHorizontalAlignment(TextHAlignment::LEFT);
    member_num_value->setAnchorPoint(Point(0, 0.5)) ;
    frame->addChild(member_num_value);
    
    //チームコメント
    string team_comment_str = teamData["comment"].string_value();
    auto team_comment = HMSFontUtil::createWithTTFBySystemFontArgument(team_comment_str, FONT_NAME_SYSTEM, 22);
    team_comment->setHorizontalAlignment(TextHAlignment::LEFT);
    team_comment->setVerticalAlignment(TextVAlignment::TOP);
    team_comment->setPosition(Point(30, 260));
    team_comment->setAnchorPoint(Point(0, 1));
    team_comment->setDimensions(560, 130);
    frame->addChild(team_comment);
    
    //勧誘コメント
    string invited_comment_str = teamData["invited_comment"].string_value();
    auto invited_comment = HMSFontUtil::createWithTTFBySystemFontArgument(invited_comment_str, FONT_NAME_SYSTEM, 22);
    invited_comment->setHorizontalAlignment(TextHAlignment::LEFT);
    invited_comment->setVerticalAlignment(TextVAlignment::TOP);
    invited_comment->setColor(Color3B::BLACK);
    invited_comment->setPosition(Point(166, 112));
    invited_comment->setAnchorPoint(Point(0, 1));
    invited_comment->setDimensions(418, 82);
    frame->addChild(invited_comment);
    
}

void TeamDetailSolicitationLayer::createButton()
{
    //承認
    auto approval_btn_on = Sprite::create("init/Team/no_belong/team_search/bt_approval.png");
    auto approval_btn_off = Sprite::create("init/Team/no_belong/team_search/bt_approval.png");
    approval_btn_on->setColor(Color3B(180, 180, 180));
    auto approval_btn = MenuItemSprite::create(approval_btn_off, approval_btn_on, [this](Ref* sender){
        this->buttonTapAction();
    });
    auto approval_menu = Menu::create(approval_btn, nullptr);
    approval_menu->setPosition(Point(frame->getContentSize().width/2 - 120, -50));
    frame->addChild(approval_menu);
    
    //拒否
    auto refusal_btn_on = Sprite::create("init/Team/no_belong/team_search/bt_refusal.png");
    auto refusal_btn_off = Sprite::create("init/Team/no_belong/team_search/bt_refusal.png");
    refusal_btn_on->setColor(Color3B(180, 180, 180));
    auto refusal_btn = MenuItemSprite::create(refusal_btn_off, refusal_btn_on, [this](Ref* sender){
        this->buttonTapAction2();
    });
    auto refusal_menu = Menu::create(refusal_btn, nullptr);
    refusal_menu->setPosition(Point(frame->getContentSize().width/2 + 120, -50));
    frame->addChild(refusal_menu);
}

void TeamDetailSolicitationLayer::buttonTapAction()
{
    string url = HOST;
    url += "team/team_petition_member.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&team_id=" + teamData["team_id"].string_value();
    url += "&type=ia";
    
    HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamDetailSolicitationLayer::buttonTapCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamDetailSolicitationLayer::buttonTapCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamTop");
        this->getParent()->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamDetailSolicitationLayer::buttonTapAction2()
{
    string url = HOST;
    url += "team/team_petition_member.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&team_id=" + teamData["team_id"].string_value();
    url += "&type=ir";
    
    HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamDetailSolicitationLayer::buttonTapCallback2);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamDetailSolicitationLayer::buttonTapCallback2(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->getParent()->addChild(layer, 9999);
        
        refreshList();
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}
