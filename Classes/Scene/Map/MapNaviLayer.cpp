#include "MapNaviLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsFontUtil.h"

#define TURN_LABEL_TAG 101
#define TURN_IMG_TAG 102
#define SITUATION_LABEL_TAG0 103
#define SITUATION_LABEL_TAG1 104
#define SITUATION_LABEL_TAG2 105
#define SITUATION_LABEL_TAG3 106
#define RESTTIME_LABEL_TAG 107
#define PERSONAL_POINT_LABEL_TAG 108
#define TEAM_POINT_LABEL_TAG 109
#define TURN_GAUGE_TAG 201

bool MapNaviLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    return true;
}

void MapNaviLayer::initialize()
{
    ratio = getVisibleRatio();
    
    _now_point[0] = 0;
    _now_point[1] = 0;
    
    //ヘッダー枠
    Sprite *img = Sprite::create("init/Map/header_bk.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 1));
    img->setPosition(convertFromDisplaySize(Point(320, DISPLAY_HEIGHT-32), 0.5, 1.0));
    this->addChild(img, -2);
    
    img = Sprite::create("init/Map/header.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 1));
    img->setPosition(convertFromDisplaySize(Point(320, DISPLAY_HEIGHT-32), 0.5, 1.0));
    this->addChild(img);
    
    //イベントPt背景
    img = Sprite::create("init/Map/pt_team.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 0.5));
    img->setPosition(convertFromDisplaySize(Point(117, 830-32), 0.5, 1.0));
    this->addChild(img);
    
    img = Sprite::create("init/Map/pt_personal.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 0.5));
    img->setPosition(convertFromDisplaySize(Point(117, 762-32), 0.5, 1.0));
    this->addChild(img);

    //スケジュール登録
    this->schedule(schedule_selector(MapNaviLayer::displayBattleMessage), 1.0f);
    this->schedule(schedule_selector(MapNaviLayer::updateEventPoint), 0.01f);
}

void MapNaviLayer::displayBattleMessage(float dt)
{
    //表示すべきものがない場合や、他を表示中には何もしない
    if(_message_list.size()==0) return;
    if(_display_message_flg==true) return;
    
    HMSLOG("message_list size=%d", _message_list.size());
    string message = _message_list.at(0);
    _display_message_flg = true;
    
    //レイヤーを準備
    Layer *layer = Layer::create();
    layer->setAnchorPoint(Point(0.5, 0.5));
    layer->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(layer);
    
    //メッセージ
    Label *label;
    Sprite *img = Sprite::create("init/Map/bk_comment.png");
    img->setScale(ratio);
    img->setScaleY(ratio);
    img->setAnchorPoint(Point(0.5, 0.5));
    //img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 110), 0.5, 0));
    //img->setPosition(Point(DISPLAY_WIDTH/2, 0));
    layer->addChild(img);
    
    label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 30*ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
    //label->setPosition(Point(DISPLAY_WIDTH/2, 0));
    layer->addChild(label);

    //layer->setScale(0);
    
    //アクション
    float t = 3.0f;
    if(_message_list.size()>1) t = 2.0f;
    ScaleTo *scale = ScaleTo::create(0.3f, 1);
    DelayTime *delay = DelayTime::create(t);
    CallFunc *expire = CallFunc::create([this]()
                                        {
                                            this->_display_message_flg = false;
                                        });//ラムダ式
    RemoveSelf *remove = RemoveSelf::create();
    layer->runAction(Sequence::create(scale, delay, expire, remove, NULL));
    
    img->setScale(0);
    scale = ScaleTo::create(0.3f, ratio);
    img->runAction(Sequence::create(scale, NULL));
    
    label->setScale(0);
    scale = ScaleTo::create(0.3f, 1);
    label->runAction(Sequence::create(scale, NULL));
    
    //リストから1件削除
    _message_list.remove(0);
}

void MapNaviLayer::addBattleMessage(string message)
{
    _message_list.insert(message);
}

void MapNaviLayer::displayCharaInfo(string message, MapCharaLayer *chara)
{
    //古いレイヤーがあれば消す
    if(_footer_layer!=nullptr) {
        Point target_point = Point(-DISPLAY_WIDTH, 0);
        MoveTo *move = MoveTo::create(0.1f, target_point);
        RemoveSelf *remove = RemoveSelf::create();
        _footer_layer->runAction(Sequence::create(move, remove, NULL));
    }
    
    //新しいレイヤーを用意する
    Layer *layer = Layer::create();
    layer->setPosition(DISPLAY_WIDTH, 0);
    this->addChild(layer);
    _footer_layer = layer;
    
    //フッター枠
    Sprite *img = Sprite::create("init/Dungeon/design/before_battle1.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 0));
    img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0));
    layer->addChild(img);
    
    Point target_point = Point(0, 0);
    MoveTo *move = MoveTo::create(0.1f, target_point);
    layer->runAction(Sequence::create(move, NULL));
    
    //キャラ画像
    string filename = "download/Chara/thumb100/" + to_string(chara->getCharaId()) + ".png";
    Sprite *img_chara = Sprite::create(filename.c_str());
    img_chara->setScale(ratio);
    img_chara->setPosition(Point(100, 80));
    img->addChild(img_chara);
    
    //キャラ名
    filename = "download/Chara/name/" + to_string(chara->getCharaId()) + ".png";
    img_chara = Sprite::create(filename.c_str());
    img_chara->setScale(0.7*ratio);
    img_chara->setPosition(Point(100, 130));
    img->addChild(img_chara);
    
    //ユーザ名表示
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(chara->getUserName(), FONT_NAME_SYSTEM, 30);
    label->setPosition(Point(100, 30));
    label->enableOutline(Color4B::BLACK, 4);
    img->addChild(label);
    
    //コメント表示
    //todoテキストBOX化
    label = HMSFontUtil::createWithTTFBySystemFontArgument(chara->getComment(), FONT_NAME_SYSTEM, 30);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(220, 100));
    img->addChild(label);

}

void MapNaviLayer::displayTurnChangeMessage(string message, bool is_player_turn)
{
    //ターン切り替え SE 
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/bash1.mp3");
    
    Sprite* frame;//ダイアログの画像
    float frame_height = 0.0f;//ダイアログの高さ
    
    frame = Sprite::create("init/Dungeon/design/battle_turn_bk_blue.png");
    frame_height = 390.0f;
 
    if(is_player_turn) {//自ターン(青)
        frame = Sprite::create("init/Dungeon/design/battle_turn_bk_blue.png");
        frame_height = 390.0f;
    } else {//相手ターン(赤)
        frame = Sprite::create("init/Dungeon/design/battle_turn_bk_red.png");
        frame_height = DISPLAY_HEIGHT - 380.0f;
    }
    
    //ダイアログの大きさを調整
    frame->setScale(ratio, 0.0f);
    frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, frame_height), 0.5, 0.5));
    //frame->setOpacity(180);
    this->addChild(frame);
    //frame->setGlobalZOrder(10.0f);
    
    //テキスト
    Label* txt_label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 30);
    float frame_internal_height = frame->getContentSize().height / 2;
    txt_label->setPosition(Point(frame->getContentSize().width + txt_label->getContentSize().width, frame_internal_height));
    frame->addChild(txt_label,2000);
    // txt_label->setGlobalZOrder(20.0f);
    //アクション作成
    frame->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.2f, ratio, ratio)),
                                      DelayTime::create(1.5f),
                                      ScaleTo::create(0.2f, ratio, 0.0f),
                                      RemoveSelf::create(true), nullptr));
    
    txt_label->runAction(Sequence::create(MoveTo::create(0.2f, Point(frame->getContentSize().width / 2 + 40, frame_internal_height)),
                                          MoveTo::create(1.5f, Point(frame->getContentSize().width / 2 - 40, frame_internal_height)),
                                          MoveTo::create(0.2f, Point(-(txt_label->getContentSize().width + 100), frame_internal_height)), nullptr));
}

void MapNaviLayer::displaySituationInfo(string player_team_name, string player_count, string enemy_team_name, string enemy_count, int turn_no, int turn_time, int turn_time_max)
{
    
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(player_team_name, FONT_NAME_SYSTEM, 22*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(16, 930-32), 0.5, 1.0));
    this->removeChildByTag(SITUATION_LABEL_TAG0);
    this->addChild(label, 0, SITUATION_LABEL_TAG0);
    
    label = HMSFontUtil::createWithTTFBySystemFontArgument(player_count, FONT_NAME_SYSTEM, 35*ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
    label->setPosition(convertFromDisplaySize(Point(233, 896-32), 0.5, 1.0));
    this->removeChildByTag(SITUATION_LABEL_TAG1);
    this->addChild(label, 0, SITUATION_LABEL_TAG1);

    label = HMSFontUtil::createWithTTFBySystemFontArgument(enemy_team_name, FONT_NAME_SYSTEM, 22*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(457, 930-32), 0.5, 1.0));
    this->removeChildByTag(SITUATION_LABEL_TAG2);
    this->addChild(label, 0, SITUATION_LABEL_TAG2);
    
    label = HMSFontUtil::createWithTTFBySystemFontArgument(enemy_count, FONT_NAME_SYSTEM, 35*ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
    label->setPosition(convertFromDisplaySize(Point(410, 896-32), 0.5, 1.0));
    this->removeChildByTag(SITUATION_LABEL_TAG3);
    this->addChild(label, 0, SITUATION_LABEL_TAG3);
    
    float percentage = (float)turn_time / (float)turn_time_max;
    Sprite *img = Sprite::create("init/Map/gauge_time.png");
    img->setAnchorPoint(Point(0, 0.5));
    img->setScaleY(ratio);
    img->setScaleX(ratio*percentage);
    int x = 37;
    if(turn_no==1) x = 485;
    img->setPosition(convertFromDisplaySize(Point(x, 889-32), 0.5, 1));
    this->removeChildByTag(TURN_GAUGE_TAG);
    this->addChild(img, -1, TURN_GAUGE_TAG);
}

void MapNaviLayer::displayRestTime(int rest_time)
{
    string text = "";
    int rest_time_ = rest_time;
//    if(rest_time_>3600) {
//        text += to_string(rest_time_ / 3600) + "時間";
//        rest_time_ = rest_time_ % 3600;
//    }
    if(rest_time_>60) {
        text += to_string(rest_time_ / 60) + "分";
        rest_time_ = rest_time_ % 60;
    }
    //text += to_string(rest_time_) + "秒";
    if(text=="残り") {
        if(rest_time_>0) text += "0分";
        else text = "-";
    }
    
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 26*ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
    label->setPosition(convertFromDisplaySize(Point(320, 902-32), 0.5, 1.0));
    this->removeChildByTag(RESTTIME_LABEL_TAG);
    this->addChild(label, 0, RESTTIME_LABEL_TAG);
}

void MapNaviLayer::setEventPoint(int personal_point, int team_point)
{
    _personal_point = personal_point;
    _team_point = team_point;
    if(_point_set_flg==false) {
        _point_set_flg = true;
        _now_point[0] = _personal_point-1;
        _now_point[1] = _team_point-1;
    }
}

void MapNaviLayer::updateEventPoint(float dt)
{    
    if(_now_point[0]<_personal_point) {
        _now_point[0]++;
        string text = to_string(_now_point[0]);
        Label *label = (Label *)this->getChildByTag(PERSONAL_POINT_LABEL_TAG);
        if(label==nullptr) {
            Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 27*ratio);
            label->setAnchorPoint(Point(1, 0.5));
            label->setPosition(convertFromDisplaySize(Point(223, 748-32), 0.5, 1.0));
            this->addChild(label, 0, PERSONAL_POINT_LABEL_TAG);
        } else {
            label->setString(text);
        }
    }
    
    if(_now_point[1]<_team_point) {
        _now_point[1]++;
        string text = to_string(_now_point[1]);
        Label *label = (Label *)this->getChildByTag(TEAM_POINT_LABEL_TAG);
        if(label==nullptr) {
            Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 27*ratio);
            label->setAnchorPoint(Point(1, 0.5));
            label->setPosition(convertFromDisplaySize(Point(223, 816-32), 0.5, 1.0));
            this->addChild(label, 0, TEAM_POINT_LABEL_TAG);
        } else {
            label->setString(text);
        }
    }
}
