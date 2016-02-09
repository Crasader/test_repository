#include "TeamInformationChangeLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsList.h"
#include "NormalScene.h"
#include "HttpConnect.h"
#include "CommentLayer.h"
#include "popupConfirmLayer.h"

USING_NS_CC;

bool TeamInformationChangeLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamInformationChangeLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamInformationChangeLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("情報設定", "TeamManagement", this);
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamInformationChangeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamInformationChangeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamInformationChangeLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamInformationChangeLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    getTeamData();
}

void TeamInformationChangeLayer::getTeamData()
{
    string url = HOST;
    url += "team/get_team_data.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    
    //HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamInformationChangeLayer::getTeamDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamInformationChangeLayer::getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    
    _json_team = Json::parse(result, err);

    setup();
}

void TeamInformationChangeLayer::setup()
{
    Sprite *popup = Sprite::create("init/Team/management/info/popup_team.png");
    popup->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    popup->setAnchorPoint(Point(0.5, 0.5)) ;
    popup->setScale(ratio);
    this->addChild(popup, O_BACK_POPUP, T_BACK_POPUP);
    
    ////チーム名////
    this->_input_team_name = _json_team["name"].string_value();
    
    //リーダー分岐
    if(SaveData::loadBool("team_leader_flg")) {
        Size text_edit_box_size = Size(510, 50);
        ui::EditBox *text_edit_box = ui::EditBox::create(text_edit_box_size, ui::Scale9Sprite::create("init/Team/management/search/popup_search_box.png"));
        text_edit_box->setFontName(FONT_NAME_SYSTEM) ;
        text_edit_box->setPosition(Point(44, 446));
        text_edit_box->setInputMode(ui::EditBox::InputMode::ANY);
        text_edit_box->setFontColor(Color4B::WHITE);
        text_edit_box->setMaxLength(8);
        text_edit_box->setOpacity(0);
        text_edit_box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        text_edit_box->setText(_input_team_name.c_str());
        text_edit_box->setAnchorPoint(Point(0, 0.5));
        popup->addChild(text_edit_box, 1, T_EDITBOX_NAME);
    } else {
        auto name = HMSFontUtil::createWithTTFBySystemFontArgument(_input_team_name, FONT_NAME_SYSTEM, 30);
        name->setPosition(Point(44, 446));
		name->setHorizontalAlignment(TextHAlignment::LEFT);
        name->setAnchorPoint(Point(0, 0.5));
        popup->addChild(name, 1);
    }
    
    /*チームコメント入力枠*/
    this->_input_team_comment = _json_team["comment"].string_value();
    
    //リーダー分岐
    if(SaveData::loadBool("team_leader_flg")) {
        //このEditBoxは透過で利用, 入力された文字列はT_EDITBOX_COMMENTタグのつけられたLabelに随時更新していく　入力中以外のeditboxのサイズは入力欄に合わせた大きさにして、入力中はSize::ZEROとする
        ui::EditBox *team_comment_text_edit_box = ui::EditBox::create(Size(510, 90), ui::Scale9Sprite::create("init/Team/management/search/popup_search_box.png"));
        team_comment_text_edit_box->setPosition(Point(44, 310));
        team_comment_text_edit_box->setInputMode(ui::EditBox::InputMode::ANY);
        team_comment_text_edit_box->setFontColor(Color4B(0,0,0,0)); //見えない文字
        team_comment_text_edit_box->setMaxLength(100);
        team_comment_text_edit_box->setDelegate(this) ;
        team_comment_text_edit_box->setOpacity(0) ;
        team_comment_text_edit_box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        team_comment_text_edit_box->setText(_input_team_comment.c_str());
        team_comment_text_edit_box->setAnchorPoint(Point(0, 0.5));
        popup->addChild(team_comment_text_edit_box, 1, T_EDITBOX_COMMENT);
    }
    
    //内容
    Label *team_comment = HMSFontUtil::createWithTTFBySystemFontArgument(_input_team_comment, FONT_NAME_SYSTEM, 20);
    team_comment->setPosition(Point(50, 310));
    team_comment->setAnchorPoint(Point(0, 0.5));
    team_comment->setDimensions(510, 90);
    popup->addChild(team_comment, 1, T_COMMENT_LABEL);

    ////ギルドタイプ////
    this->_team_type = _json_team["team_type"].string_value();
    if(this->_team_type=="a") {
        this->optionChange(101);
    }
    else if(this->_team_type=="b") {
        this->optionChange(102);
    }
    else if(this->_team_type=="c") {
        this->optionChange(103);
    }
    
    ////プレイタイプ////
    this->_play_type = _json_team["play_type"].string_value();
    if(this->_play_type=="a") {
        this->optionChange(201);
    }
    else if(this->_play_type=="b") {
        this->optionChange(202);
    }
    else if(this->_play_type=="c") {
        this->optionChange(203);
    }
    
    //設定
    if(SaveData::loadBool("team_leader_flg")) {
        Sprite *save = Sprite::create("init/Team/management/info/bt_save_settings.png");
        save->setPosition(convertFromDisplaySize(Point(320, FOOTER_SIZE+50), 0.5, 0));
        save->setScale(ratio);
        this->addChild(save, 1, T_SAVE);
    }
    
}

//選択項目変更時処理
void TeamInformationChangeLayer::optionChange(int tap_tag)
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

//タッチイベント処理
bool TeamInformationChangeLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamInformationChangeLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamInformationChangeLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
    //リーダー以外は参照のみ
    if(!SaveData::loadBool("team_leader_flg")) return;
    
    
    Sprite *popup = (Sprite *)getChildByTag(T_BACK_POPUP) ;
    if(!popup) return ;
    
    ////ギルドタイプ////
    //初心者
    Sprite *team_type_a = (Sprite*)popup->getChildByTag(101);
    Rect team_type_a_rect = team_type_a->boundingBox();
    if(Utility::containsPoint(team_type_a, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(101);
        return;
    }
    //中級者
    Sprite *team_type_b = (Sprite*)popup->getChildByTag(102);
    Rect team_type_b_rect = team_type_b->boundingBox();
    if(Utility::containsPoint(team_type_b, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(102);
        return;
    }
    //上級者
    Sprite *team_type_c = (Sprite*)popup->getChildByTag(103);
    Rect team_type_c_rect = team_type_c->boundingBox();
    if(Utility::containsPoint(team_type_c, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(103);
        return;
    }
    
    ////プレイタイプ////
    //まったり
    Sprite *play_type_a = (Sprite*)popup->getChildByTag(201);
    Rect play_type_a_rect = play_type_a->boundingBox();
    if(Utility::containsPoint(play_type_a, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(201);
        return;
    }
    //ほどほど
    Sprite *play_type_b = (Sprite*)popup->getChildByTag(202);
    Rect play_type_b_rect = play_type_b->boundingBox();
    if(Utility::containsPoint(play_type_b, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(202);
        return;
    }
    //がっつり
    Sprite *play_type_c = (Sprite*)popup->getChildByTag(203);
    Rect play_type_c_rect = play_type_c->boundingBox();
    if(Utility::containsPoint(play_type_c, touch->getLocation())){
        SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        this->optionChange(203);
        return;
    }
    
    
    //変更実行
    Sprite *save = (Sprite*)this->getChildByTag(T_SAVE);
    Rect save_rect = save->boundingBox();
    if(save_rect.containsPoint(touch->getLocation())) {
        ui::EditBox *editBox = (ui::EditBox *)popup->getChildByTag(T_EDITBOX_NAME);
        _input_team_name = editBox->getText();
        editBox = (ui::EditBox *)popup->getChildByTag(T_EDITBOX_COMMENT);
        _input_team_comment = editBox->getText();
        
        //未入力チェック
        if(this->_input_team_name=="") {
            //CommentLayer *comment = CommentLayer::create();
            string text = "チーム名が未入力です";
            //comment->setText(text);
            popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
            this->addChild(comment, 9999);
        } else {
            string text = "設定情報を変更してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_information_change.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                
                //チーム名
                url += "&team_name=";
                url += URLEncode(_input_team_name);
                //チームコメント
                url += "&team_comment=";
                url += URLEncode(_input_team_comment);
                //チームタイプ
                url += "&team_type=";
                url += _team_type;
                //プレイタイプ
                url += "&play_type=";
                url += _play_type;
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamInformationChangeLayer::httpCallback);
                HttpConnect::sendHttpRequest(this, url, selector);

            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
        }
    }
}
void TeamInformationChangeLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TeamInformationChangeLayer::httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamManagement");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamInformationChangeLayer::editBoxEditingDidBegin(ui::EditBox* editBox) {
    //入力開始時大きさをゼロにする
    editBox->setContentSize(Size::ZERO) ;
}
void TeamInformationChangeLayer::editBoxEditingDidEnd(ui::EditBox* editBox) {
    //入力開始時大きさをエディットボックスのサイズにする
    editBox->setContentSize(Size(510, 90)) ;
}

void TeamInformationChangeLayer::editBoxTextChanged(ui::EditBox* editBox, const std::string& text)
{
    Sprite *popup = (Sprite *)getChildByTag(T_BACK_POPUP) ;
    if(!popup) return ;
    
    Label *label = (Label *)popup->getChildByTag(T_COMMENT_LABEL) ;
    label->setString(text) ;
}

void TeamInformationChangeLayer::editBoxReturn(ui::EditBox* editBox) { }
