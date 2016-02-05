#include "TournamentRemainTimeSprite.h"

TournamentRemainTimeSprite::TournamentRemainTimeSprite()
:_total_battle_number(-1)
,_now_battle_number(0)
,_remain_time(0)
{
}

TournamentRemainTimeSprite* TournamentRemainTimeSprite::create(int total_battle_number, int now_battle_number, Node* node)
{
    TournamentRemainTimeSprite *sprite = new TournamentRemainTimeSprite();
    if (sprite && sprite->init(total_battle_number, now_battle_number, node))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentRemainTimeSprite::init(int total_battle_number, int now_battle_number, Node* node)
{
    if ( !Sprite::initWithFile("init/Home/Event/TournamentEvent/popup_tournament.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    _total_battle_number = total_battle_number;
    _now_battle_number = now_battle_number;
    _node = node;
    
    return true;
}

void TournamentRemainTimeSprite::onEnter()
{
    Sprite::onEnter();
    
    //残り時間の初期値を設定
    time_t now_time;
    time(&now_time);
    time_t now_timestamp = now_time - (time_t)SaveData::loadInt("server_diff_time");
    _remain_time = NormalScene::_tournament_next_battle_time - now_timestamp;
    
    //表示上0秒以下だったら0秒に
    if(_remain_time<0)_remain_time = 0;

    
    //次戦のラウンドラベルの作成
    createNextRoundLabel();
    
    //残り時間作成
    createRemainTimeLabel();
    
    //スケジューラの設定
    schedule(schedule_selector(TournamentRemainTimeSprite::updateTournamentNextBattleRemainTimeLabel), 1.0f);
}

void TournamentRemainTimeSprite::createNextRoundLabel()
{
    TournamentEventTopSprite::RoundStatus remain_battle = (TournamentEventTopSprite::RoundStatus)(_total_battle_number - _now_battle_number);
    
    string round_text;
    switch (remain_battle) {
        case TournamentEventTopSprite::None:          round_text = "準備中";  break;
        case TournamentEventTopSprite::Final:         round_text = "決勝";    break;
        case TournamentEventTopSprite::SemiFinal:     round_text = "準決勝";  break;
        case TournamentEventTopSprite::QuarterFinals: round_text = "準々決勝"; break;
        default: round_text = "第" + toString<int>(_now_battle_number) + "回戦"; break;
    }
    
    auto round_label = HMSFontUtil::createWithTTFBySystemFontArgument(round_text, FONT_NAME_SYSTEM, 40);
    round_label->setPosition(Point(this->getContentSize().width / 2, 156));
    round_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(round_label);
}

void TournamentRemainTimeSprite::createRemainTimeLabel()
{
    auto until_label = HMSFontUtil::createWithTTFBySystemFontArgument("開始時間まで", FONT_NAME_SYSTEM, 26);
    until_label->setPosition(Point(this->getContentSize().width / 2, 116));
    until_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(until_label);
    
    string text = createRemainStr(_remain_time);
    auto time_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 52);
    time_label->setPosition(Point(this->getContentSize().width / 2, 64));
    time_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    time_label->setColor(Color3B(255, 255, 0));
    this->addChild(time_label, Z_Remain_Label, T_Remain_Label);
}

string TournamentRemainTimeSprite::createRemainStr(time_t remain_time)
{
    int minute = (int)remain_time / 60; //分
    int second = (int)remain_time % 60; //秒
    
    string text;
//    text = toString<int>(minute) + "分" + toString<int>(second) + "秒";
    if (minute > 0) text = toString<int>(minute) + "分";
    else text = toString<int>(second) + "秒";
    
    return text;
}

void TournamentRemainTimeSprite::updateTournamentNextBattleRemainTimeLabel(float dt)
{
    
    //残り時間の初期値を設定
    time_t now_time;
    time(&now_time);
    time_t now_timestamp = now_time - (time_t)SaveData::loadInt("server_diff_time");
    _remain_time = NormalScene::_tournament_next_battle_time - now_timestamp;

    
    //トーナメント次戦対戦時刻の更新
    auto time_label = (Label *)this->getChildByTag(T_Remain_Label);
    time_label->setString(createRemainStr(_remain_time));
    
    if (_remain_time <= 0) {
        unschedule(schedule_selector(TournamentRemainTimeSprite::updateTournamentNextBattleRemainTimeLabel));
        
        //トーナメント開始
        showTournamentStartLayer();
        
        //閉じる
        closeAction();
    }
    
    _remain_time--;
}

void TournamentRemainTimeSprite::showTournamentStartLayer()
{
    //トーナメント開始の表示
    auto animmation_layer = TournamentStartAnimationLayer::create(_node);
    _node->addChild(animmation_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
}

void TournamentRemainTimeSprite::closeAction()
{
    //閉じるアクション（見えなくするだけ）
    auto scale = ScaleTo::create(0.8f, 0);
    auto ease  = EaseBackIn::create(scale);
    this->runAction(Sequence::create(ease, nullptr));
//    this->runAction(Sequence::create(ease, RemoveSelf::create(true), nullptr));
}

