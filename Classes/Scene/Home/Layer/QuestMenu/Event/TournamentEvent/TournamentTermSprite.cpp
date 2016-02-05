#include "TournamentTermSprite.hpp"

TournamentTermSprite::TournamentTermSprite()
:_wait_start_date_timestamp(0)
,_remain_time(0)
{
}

TournamentTermSprite* TournamentTermSprite::create()
{
    TournamentTermSprite *sprite = new TournamentTermSprite();
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentTermSprite::init()
{
    if ( !Sprite::initWithFile("init/Home/Event/TournamentEvent/time_left.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    _wait_start_date_timestamp = NormalScene::_tournament_next_battle_time;
    CCLOG("wait_next_battle_time = %ld", _wait_start_date_timestamp);
    
    
    return true;
}

void TournamentTermSprite::onEnter()
{
    Sprite::onEnter();
    
    //残り時間の計算
    calcTime();
    
    //ラベルの作成
    createLabel();
    
    //時間の更新処理
    schedule(schedule_selector(TournamentTermSprite::updateRemainTime), 1.0);
}

void TournamentTermSprite::calcTime()
{
    //時間の計算
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    _remain_time = _wait_start_date_timestamp - now_time;
}

void TournamentTermSprite::createLabel()
{
    //トーナメント終了文字列
    string text = "受付期間終了まで";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 21, 150));
    label->setPosition(this->getContentSize().width / 2 , 63);
    label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(label, Z_Term_Label, T_Term_Label);
    
    //トーナメント終了時刻のラベル
    string term_text = createRemainTimeStr();
    auto tournament_time_label = HMSFontUtil::createWithTTFBySystemFontArgument(term_text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(term_text, 22, 140));
    tournament_time_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height / 2 - 12));
    tournament_time_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    tournament_time_label->setColor(Color3B(255, 10, 10));
    this->addChild(tournament_time_label, Z_Time, T_Time);
}

string TournamentTermSprite::createRemainTimeStr()
{
    //残り時間文字列の作成
    string remain_text;
    
    int minute = (int)_remain_time / 60; //分
    int second = (int)_remain_time % 60; //秒
    
    //2桁で表示させる処理
    if (minute >= 0 && second >= 0)
    {
        string second_text = toString<int>(second);
        if (second < 10) second_text = "0" + toString<int>(second);
        
        remain_text = "あと" + toString<int>(minute) + ":" + second_text;
    }
    
    return remain_text;
}

void TournamentTermSprite::closeAction()
{
    //トーナメント終了時刻画像を閉じる
    auto move = MoveBy::create(MOVING_TIME, Point(_contentSize.width, 0));
    this->runAction(Sequence::create(move, RemoveSelf::create(true), nullptr));
}

void TournamentTermSprite::updateRemainTime(float dt)
{
    //トーナメント開始まで（エントリー終了まで）の時間を更新する
    auto label = (Label *)this->getChildByTag(T_Time);
    
    //ラベルの更新
//    _remain_time--;
    
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    _remain_time = _wait_start_date_timestamp - now_time;

    
    label->setString(createRemainTimeStr());
    
    //時間を過ぎたら閉じる
    if (_remain_time <= 0) closeAction();
}

