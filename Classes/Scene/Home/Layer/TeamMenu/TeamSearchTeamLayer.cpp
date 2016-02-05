#include "TeamSearchTeamLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
//#include "UserDefaultUtil.h"
#include "HttpConnect.h"
#include "Utility.h"
#include "NormalScene.h"

USING_NS_CC;

bool TeamSearchTeamLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamSearchTeamLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamSearchTeamLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム検索", "TeamNoBelong", this);
    
    setup();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamSearchTeamLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamSearchTeamLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamSearchTeamLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamSearchTeamLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamSearchTeamLayer::setup()
{
    Sprite *popup = Sprite::create("init/Team/no_belong/team_search/popup_614_400_2.png");
    popup->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    popup->setAnchorPoint(Point(0.5, 0.5)) ;
    popup->setScale(ratio);
    this->addChild(popup, 1, T_BACK_POPUP);
    
    ////ギルドタイプ初期値設定
    this->optionChange(101);
    
    ////プレイタイプ初期値設定
    this->optionChange(201);
    
    //名前入力
    Size team_name_text_edit_box_size = Size(532, 68);
    ui::EditBox *team_name_text_edit_box = ui::EditBox::create(team_name_text_edit_box_size, ui::Scale9Sprite::create("init/Team/no_belong/create/entry_column.png"));
    team_name_text_edit_box->setPosition(Point(popup->getContentSize().width/2,295));
    team_name_text_edit_box->setPlaceholderFontColor(Color3B::GRAY) ;
    team_name_text_edit_box->setPlaceHolder("チーム名を入力") ;
    team_name_text_edit_box->setInputMode(ui::EditBox::InputMode::ANY);
    team_name_text_edit_box->setFontColor(Color4B::WHITE);
    team_name_text_edit_box->setMaxLength(10);
    team_name_text_edit_box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    popup->addChild(team_name_text_edit_box, 1, T_EDITBOX_NAME);
    
    //検索
    Sprite *search = Sprite::create("init/Team/no_belong/team_search/bt_search_conditions.png");
    search->setPosition(convertFromDisplaySize(Point(320, FOOTER_SIZE+50), 0.5, 0));
    search->setScale(ratio);
    this->addChild(search, 1, T_SEARCH);
}

void TeamSearchTeamLayer::optionChange(int tap_tag)
{
    Sprite *popup = (Sprite *)this->getChildByTag(T_BACK_POPUP) ;
    
    popup->removeChildByTag(101, true);
    popup->removeChildByTag(102, true);
    popup->removeChildByTag(103, true);
    popup->removeChildByTag(201, true);
    popup->removeChildByTag(202, true);
    popup->removeChildByTag(203, true);
    
    //ポジション
    Point position_101 = Point(129, 59);
    Point position_102 = Point(311, 59);
    Point position_103 = Point(493, 59);
    Point position_201 = Point(129, 173);
    Point position_202 = Point(311, 173);
    Point position_203 = Point(493, 173);
    
    //ギルドタイプ//
    //初心者
    if(tap_tag==101) {
        this->_team_type = "a";
    }
    //中級者
    else if(tap_tag==102) {
        this->_team_type = "b";
    }
    //上級者
    else if(tap_tag==103) {
        this->_team_type = "c";
    }
    //プレイタイプ//
    //まったり
    else if(tap_tag==201) {
        this->_play_type = "a";
    }
    //ほどほど
    else if(tap_tag==202) {
        this->_play_type = "b";
    }
    //がっつり
    else if(tap_tag==203) {
        this->_play_type = "c";
    }
    
    ////ボタン配置////
    string team_type_a = "init/Team/management/info/bt_guild_type_a_";
    string team_type_b = "init/Team/management/info/bt_guild_type_b_";
    string team_type_c = "init/Team/management/info/bt_guild_type_c_";
    string play_type_a = "init/Team/management/info/bt_play_type_a_";
    string play_type_b = "init/Team/management/info/bt_play_type_b_";
    string play_type_c = "init/Team/management/info/bt_play_type_c_";
    //ギルドタイプ
    if(_team_type=="a") {
        team_type_a += "on";
        team_type_b += "off";
        team_type_c += "off";
    }
    else if(_team_type=="b") {
        team_type_a += "off";
        team_type_b += "on";
        team_type_c += "off";
    }
    else if(_team_type=="c") {
        team_type_a += "off";
        team_type_b += "off";
        team_type_c += "on";
    }
    else {
        team_type_a += "off";
        team_type_b += "off";
        team_type_c += "off";
    }
    team_type_a += ".png";
    team_type_b += ".png";
    team_type_c += ".png";
    //プレイタイプ
    if(_play_type=="a") {
        play_type_a += "on";
        play_type_b += "off";
        play_type_c += "off";
    }
    else if(_play_type=="b") {
        play_type_a += "off";
        play_type_b += "on";
        play_type_c += "off";
    }
    else if(_play_type=="c") {
        play_type_a += "off";
        play_type_b += "off";
        play_type_c += "on";
    }
    else {
        play_type_a += "off";
        play_type_b += "off";
        play_type_c += "off";
    }
    play_type_a += ".png";
    play_type_b += ".png";
    play_type_c += ".png";
    
    Sprite *bt_team_type_a = Sprite::create(team_type_a.c_str());
    bt_team_type_a->setPosition(position_101);
    popup->addChild(bt_team_type_a, 1, 101);
    
    Sprite *bt_team_type_b = Sprite::create(team_type_b.c_str());
    bt_team_type_b->setPosition(position_102);
    popup->addChild(bt_team_type_b, 1, 102);
    
    Sprite *bt_team_type_c = Sprite::create(team_type_c.c_str());
    bt_team_type_c->setPosition(position_103);
    popup->addChild(bt_team_type_c, 1, 103);
    
    Sprite *bt_play_type_a = Sprite::create(play_type_a.c_str());
    bt_play_type_a->setPosition(position_201);
    popup->addChild(bt_play_type_a, 1, 201);
    
    Sprite *bt_play_type_b = Sprite::create(play_type_b.c_str());
    bt_play_type_b->setPosition(position_202);
    popup->addChild(bt_play_type_b, 1, 202);
    
    Sprite *bt_play_type_c = Sprite::create(play_type_c.c_str());
    bt_play_type_c->setPosition(position_203);
    popup->addChild(bt_play_type_c, 1, 203);
}

bool TeamSearchTeamLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamSearchTeamLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamSearchTeamLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    ////ギルドタイプ////
    Sprite *popup = (Sprite *)getChildByTag(T_BACK_POPUP) ;
    if(!popup) return ;
    
    //初心者
    Sprite *team_type_a = (Sprite*)popup->getChildByTag(101);
    Rect team_type_a_rect = team_type_a->boundingBox();
    if(Utility::containsPoint(team_type_a, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(101);
        return;
    }
    //中級者
    Sprite *team_type_b = (Sprite*)popup->getChildByTag(102);
    Rect team_type_b_rect = team_type_b->boundingBox();
    if(Utility::containsPoint(team_type_b, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(102);
        return;
    }
    //上級者
    Sprite *team_type_c = (Sprite*)popup->getChildByTag(103);
    Rect team_type_c_rect = team_type_c->boundingBox();
    if(Utility::containsPoint(team_type_c, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(103);
        return;
    }
    
    ////プレイタイプ////
    //まったり
    Sprite *play_type_a = (Sprite*)popup->getChildByTag(201);
    Rect play_type_a_rect = play_type_a->boundingBox();
    if(Utility::containsPoint(play_type_a, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(201);
        return;
    }
    //ほどほど
    Sprite *play_type_b = (Sprite*)popup->getChildByTag(202);
    Rect play_type_b_rect = play_type_b->boundingBox();
    if(Utility::containsPoint(play_type_b, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(202);
        return;
    }
    //がっつり
    Sprite *play_type_c = (Sprite*)popup->getChildByTag(203);
    Rect play_type_c_rect = play_type_c->boundingBox();
    if(Utility::containsPoint(play_type_c, touch->getLocation())) {
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(203);
        return;
    }
    
    //検索
    Sprite *search = (Sprite*)this->getChildByTag(T_SEARCH);
    Rect search_rect = search->boundingBox();
    if(search_rect.containsPoint(touch->getLocation())) {
        ui::EditBox *editBox = (ui::EditBox *)popup->getChildByTag(T_EDITBOX_NAME);
        _input_team_name = editBox->getText();
        
        string url = HOST;
        url += "team/team_search.php";
        url += "?user_master_id=" + SaveData::loadString("user_master_id");
        
        //チーム名
        url += "&team_name=";
        url += URLEncode(_input_team_name);
        //チームタイプ
        url += "&team_type=";
        url += _team_type;
        //プレイタイプ
        url += "&play_type=";
        url += _play_type;
        
        //HMSLOG("url=%s", url.c_str());
        auto selector = httpresponse_selector(TeamSearchTeamLayer::httpCallback);
        HttpConnect::sendHttpRequest(this, url, selector);
    }
}

void TeamSearchTeamLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

//httpコールバック
void TeamSearchTeamLayer::httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    //Json json = Json::parse(result, err);

    if(result=="") {
        string text = "条件に該当するチームはありません。";
        popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
        this->addChild(comment, 9999);
    } else {
        SaveData::saveString("team_search_result", result);
        SaveData::saveString("team_list_mode", "search");
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("TeamList");
    }
}