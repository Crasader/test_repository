#include "TournamentEventTopSprite.h"

TournamentEventTopSprite::TournamentEventTopSprite()
{
}

TournamentEventTopSprite* TournamentEventTopSprite::create(json11::Json json, string &image_path)
{
    TournamentEventTopSprite *sprite = new TournamentEventTopSprite();
    if (sprite && sprite->init(json, image_path))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventTopSprite::init(json11::Json json, string &image_path)
{
    if ( !Sprite::initWithFile(image_path) )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    
    return true;
}

void TournamentEventTopSprite::setupData(json11::Json json)
{
    //データの設定
    setTournamentEventListId(json["tournament_event_list_id"].string_value());
    setTournamentName(json["tournament_name"].string_value());
    setEnemyName(json["enemy_name"].string_value());
    setWaitStartDate(json["wait_start_date"].string_value());
    setTotalBattleNumber(json["total_battle_number"].string_value());
    setNowBattleNumber(json["now_battle_number"].string_value());
}

void TournamentEventTopSprite::onEnter()
{
    Sprite::onEnter();
    
    //トーナメント名の作成
    createTournamentName();
    
    //何回戦の表示
    showRoundName();
    
    //対戦日時の表示
    showEventOpenDate();
    
    //対戦相手の名前の作成
    createEnemyName();
    
    //次戦のラウンド数を表示
    showRoundName();
}

void TournamentEventTopSprite::createTournamentName()
{
    //トーナメント名の作成
    auto tournament_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_tournament_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_tournament_name, 34, 380));
    tournament_name_label->setColor(Color3B(255, 234, 0));
    tournament_name_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 28));
    tournament_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(tournament_name_label, Z_Tournament_Name, T_Tournament_Name);
}

void TournamentEventTopSprite::showRoundName()
{
    //次戦のラウンド数を計算し表示する
    RoundStatus remain_battle = (RoundStatus)(fromString<int>(_total_battle_number) - fromString<int>(_now_battle_number));
    
    string round_text;
    switch (remain_battle) {
        case None:          round_text = "トーナメント表作成中";  break;
        case Final:         round_text = "決勝";    break;
        case SemiFinal:     round_text = "準決勝";  break;
        case QuarterFinals: round_text = "準々決勝"; break;
        default: round_text = "第" + _now_battle_number + "回戦"; break;
    }

    auto round_label = HMSFontUtil::createWithTTFBySystemFontArgument(round_text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(round_text, 34, 375));
    round_label->setColor(Color3B(255, 234, 0));
    round_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 84));
    round_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(round_label, Z_Round_Label, T_Round_Label);
}

void TournamentEventTopSprite::showEventOpenDate()
{
    //対戦日時の表示
    auto event_open_label = HMSFontUtil::createWithTTFBySystemFontArgument(_wait_start_date, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_enemy_name, 28, 370));
    event_open_label->setAnchorPoint(Point(0, 0.5));
    event_open_label->setPosition(Point(186, this->getContentSize().height - 140));
    event_open_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(event_open_label, Z_Event_Open_Label, T_Event_Open_Label);
}

string TournamentEventTopSprite::createEventOpenDate()
{
    //トーナメント開催日時文字列の作成
    string start_date = _wait_start_date;
    start_date.replace(10, 9, "日");
    start_date.replace(7, 1, "月");
    start_date.replace(4, 1, "年 ");
    
    string start_time = _wait_start_date;
    start_time.replace(16, 3, "");
    start_time.replace(0, 11, "");
    
    string event_open_time = start_date;
    HMSLOG("対戦日時 = %s", event_open_time.c_str());
    
    return event_open_time;
}

void TournamentEventTopSprite::createEnemyName()
{
    //対戦相手の名前
    auto enemy_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_enemy_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_enemy_name, 28, 370));
    enemy_name_label->setAnchorPoint(Point(0, 0.5));
    enemy_name_label->setPosition(Point(186, this->getContentSize().height - 196));
    enemy_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(enemy_name_label, Z_Enemy_Name, T_Enemy_Name);
}

