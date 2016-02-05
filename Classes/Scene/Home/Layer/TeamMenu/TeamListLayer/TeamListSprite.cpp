//
//  TeamListSprite.cpp
//  war
//
//  Created by OgasawaraJun on 2015/12/03.
//
//

#include "Utility.h"
#include "TeamListSprite.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"


TeamListSprite* TeamListSprite::create(Json teamData, function<void(Json)> touchAction){
    TeamListSprite *sprite = new TeamListSprite();
    if (sprite && sprite->init(teamData, touchAction))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TeamListSprite::init(Json teamData, function<void(Json)> touchAction){
    if ( !Sprite::initWithFile("init/Team/management/search/popup_team_s.png") )    return false;
    
    ratio = getVisibleRatio();
    this->teamData = teamData;
    this->touchAction = touchAction;
    this->movingFlg = false;
    
    return true;
}


void TeamListSprite::onEnter()
{
    Sprite::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    createTeamNameLabel();
    createTeamLeaderNameLabel();
    createTeamTypeLabel();
    createPlayTypeLabel();
    createMemberNumLabel();
}

void TeamListSprite::createTeamNameLabel(){
    teamNameLabel = HMSFontUtil::createWithTTFBySystemFontArgument(teamData["name"].string_value(), FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(teamData["name"].string_value(), 28, 180));
    teamNameLabel->setPosition(Point(48, this->getContentSize().height/2 + 12));
    teamNameLabel->setAnchorPoint(Point(0, 0.5));
    this->addChild(teamNameLabel, ZOrder::Z_Team_Name, Tag::T_Team_Name);
    
}

void TeamListSprite::createTeamLeaderNameLabel(){
    teamLeaderNameLabel = HMSFontUtil::createWithTTFBySystemFontArgument(teamData["admin_user_name"].string_value(), FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(teamData["admin_user_name"].string_value(), 20, 180));
    teamLeaderNameLabel->setPosition(Point(160, this->getContentSize().height/2 - 31));
    teamLeaderNameLabel->setAnchorPoint(Point(0, 0.5));
    this->addChild(teamLeaderNameLabel, ZOrder::Z_Team_Leader_Name, Tag::T_Team_Leader_Name);
    
}


void TeamListSprite::createTeamTypeLabel(){
    //ギルドタイプ//
    string teamTypeStr = "" ;
    teamType = teamData["team_type"].string_value();
    if(teamType == "a")     teamTypeStr = "初心者歓迎" ;
    else if(teamType == "b")teamTypeStr = "中級者限定" ;
    else if(teamType == "c")teamTypeStr = "上級者求む" ;
    else                    teamTypeStr = "----" ;
    
    teamTypeLabel = HMSFontUtil::createWithTTFBySystemFontArgument(teamTypeStr, FONT_NAME_SYSTEM, 23);
    teamTypeLabel->setPosition(Point(420, this->getContentSize().height/2 + 20));
    teamTypeLabel->setHorizontalAlignment(TextHAlignment::LEFT) ;
    teamTypeLabel->setVerticalAlignment(TextVAlignment::TOP);
    teamTypeLabel->setAnchorPoint(Point(0.5, 0.5));
    this->addChild(teamTypeLabel, ZOrder::Z_Team_type_Label, Tag::T_Team_type_Label);
    
}

void TeamListSprite::createPlayTypeLabel(){
    //プレイタイプ//
    string playTypeStr = "" ;
    playType = teamData["play_type"].string_value();
    if(playType == "a")     playTypeStr = "まったり" ;
    else if(playType == "b")playTypeStr = "ほどほど" ;
    else if(playType == "c")playTypeStr = "がっつり" ;
    else                    playTypeStr = "----" ;
    
    playTypeLabel = HMSFontUtil::createWithTTFBySystemFontArgument(playTypeStr, FONT_NAME_SYSTEM, 23);
    playTypeLabel->setPosition(Point(420, this->getContentSize().height/2 - 27));
    playTypeLabel->setHorizontalAlignment(TextHAlignment::LEFT) ;
    playTypeLabel->setVerticalAlignment(TextVAlignment::TOP);
    playTypeLabel->setAnchorPoint(Point(0.5, 0.5));
    this->addChild(playTypeLabel, ZOrder::Z_Play_type_Label, Tag::T_Play_type_Label);
    
}


void TeamListSprite::createMemberNumLabel(){
    string TeamMemberNum = teamData["member_count"].string_value();
    TeamMemberNum += "/";
    TeamMemberNum += teamData["max_member_count"].string_value();
    memberNumLabel = HMSFontUtil::createWithTTFBySystemFontArgument(TeamMemberNum, FONT_NAME_SYSTEM, 22);
    memberNumLabel->setPosition(Point(562, this->getContentSize().height/2 - 23));
    memberNumLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    memberNumLabel->setAnchorPoint(Point(0.5, 0.5)) ;
    this->addChild(memberNumLabel);
}


void TeamListSprite::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TeamListSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamListSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamListSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamListSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool TeamListSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    
    movingFlg = false;
    
    return true;
}
void TeamListSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (abs(touch->getStartLocation().x - touch->getLocation().x) > 20
        || abs(touch->getStartLocation().y- touch->getLocation().y) > 20) {
        movingFlg = true;
    }
}

void TeamListSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //自分の絶対座標の値を取得
    Vec2 spWorldPoint = this->convertToWorldSpace(Point::ZERO);
    Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, this->getContentSize().width * ratio, this->getContentSize().height * ratio);
    
    if(rect.containsPoint(touch->getLocation()) && !movingFlg) {
        this->touchAction(teamData);
    }
}
void TeamListSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}