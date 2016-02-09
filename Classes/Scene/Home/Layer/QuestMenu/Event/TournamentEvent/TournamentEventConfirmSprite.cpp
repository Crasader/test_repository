#include "TournamentEventConfirmSprite.h"

TournamentEventConfirmSprite::TournamentEventConfirmSprite()
{
}

TournamentEventConfirmSprite* TournamentEventConfirmSprite::create(json11::Json json, Status status)
{
    TournamentEventConfirmSprite *sprite = new TournamentEventConfirmSprite();
    if (sprite && sprite->init(json, status))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventConfirmSprite::init(json11::Json json, Status status)
{
    string base_path = "init/Home/Event/TournamentEvent/bk_tournament3.png";
    if (status == Cancel || status == Top_info) base_path = "init/Home/Event/TournamentEvent/bk_tournament4.png";
    
    if ( !Sprite::initWithFile(base_path) )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    _status = status;
    
    return true;
}

void TournamentEventConfirmSprite::setupData(json11::Json json)
{
    //データの設定
    setTournamentEventListId(json["tournament_event_list_id"].string_value());
    setTournamentName(json["name"].string_value());
    setConditionName(json["condition_name"].string_value());
    setTournamentStartDate(json["tournament_start_date"].string_value());
    setDeckName(json["deck_name"].string_value());
}

void TournamentEventConfirmSprite::onEnter()
{
    Sprite::onEnter();
    
    //トーナメント名の作成
    createTournamentName();
    
    //参加条件の作成、表示
    createConditionLabel();
    
    //対戦日時の表示
    showEventOpenDate();
    
    //使用デッキ名の表示
    showUseDeckName();
    
    //注意文の表示
    if (_status == Entry) showCautionLabel();
}

void TournamentEventConfirmSprite::createTournamentName()
{
    //トーナメント名の作成
    auto tournament_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_tournament_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_tournament_name, 34, 400));
    tournament_name_label->setColor(Color3B(255, 234, 0));
    tournament_name_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 36));
    tournament_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(tournament_name_label, Z_Tournament_Name, T_Tournament_Name);
}

void TournamentEventConfirmSprite::createConditionLabel()
{
    //参加条件
    auto condition_label = HMSFontUtil::createWithTTFBySystemFontArgument(_condition_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, 29, 375));
    condition_label->setAnchorPoint(Point(0, 0.5));
    condition_label->setPosition(Point(186, this->getContentSize().height - 112));
    condition_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(condition_label, Z_Tournament_Name, T_Tournament_Name);
}

void TournamentEventConfirmSprite::showEventOpenDate()
{
    //対戦日時の表示
    auto event_open_label = HMSFontUtil::createWithTTFBySystemFontArgument(_tournament_start_date, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, 29, 375));
    event_open_label->setAnchorPoint(Point(0, 0.5));
    event_open_label->setPosition(Point(186, this->getContentSize().height - 186));
    event_open_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(event_open_label, Z_Event_Open_Label, T_Event_Open_Label);
}

void TournamentEventConfirmSprite::showUseDeckName()
{
    //使用デッキ名の表示
    auto deck_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_deck_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_deck_name, 29, 375));
    deck_name_label->setAnchorPoint(Point(0, 0.5));
    deck_name_label->setPosition(Point(186, this->getContentSize().height - 270));
    deck_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(deck_name_label, Z_Use_Deck_Name, T_Use_Deck_Name);
}

void TournamentEventConfirmSprite::showCautionLabel()
{
    //注意文ラベルの表示
    string caution_text = "※マッチングの相手と時間は、受付締め切り後に\n　イベントトップ画面に表示されます。";
    auto caution_label = HMSFontUtil::createWithTTFBySystemFontArgument(caution_text, FONT_NAME_SYSTEM, 24);
    caution_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 346));
    caution_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(caution_label, Z_Caution, T_Caution);
}