#include "TeamInvitationLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HttpConnect.h"
#include "NormalScene.h"

USING_NS_CC;

//#define TAG_BACK 11
#define TAG_SEARCH 12
#define TAG_EDITBOX_ID 21
#define TAG_EDITBOX_NAME 22
#define TAG_EDITBOX_LEVEL_MIN 23
#define TAG_EDITBOX_LEVEL_MAX 24
#define TAG_BACK 25


bool TeamInvitationLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamInvitationLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamInvitationLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("勧誘", "TeamManagement", this);

    setup();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamInvitationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamInvitationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamInvitationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamInvitationLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamInvitationLayer::setup()
{
    _input_friend_id = SaveData::loadString("team_invitation_friend_id");
    _input_user_name = SaveData::loadString("team_invitation_user_name");
//    _input_user_level_min = SaveData::loadString("team_invitation_user_level_min");
//    _input_user_level_max = SaveData::loadString("team_invitation_user_level_max");
    
    Sprite *back = Sprite::create("init/Team/management/search/popup_search.png");
    back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    back->setScale(ratio);
    this->addChild(back, 0, TAG_BACK);
    
    ui::EditBox *text_edit_box = ui::EditBox::create(Size(520, 53), ui::Scale9Sprite::create("init/Team/management/search/popup_search.png"));
    text_edit_box->setOpacity(0) ;
    text_edit_box->setPosition(Point(47, 193));
    text_edit_box->setInputMode(ui::EditBox::InputMode::NUMERIC);
    text_edit_box->setFontColor(Color4B::WHITE);
    text_edit_box->setMaxLength(10);
    text_edit_box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    text_edit_box->setText(_input_friend_id.c_str());
    text_edit_box->setAnchorPoint(Point(0, 0.5));
    back->addChild(text_edit_box, 1, TAG_EDITBOX_ID);
    
    ui::EditBox *user_name_text_edit_box = ui::EditBox::create(Size(520, 53), ui::Scale9Sprite::create("init/Team/management/search/popup_search.png"));
    user_name_text_edit_box->setOpacity(0) ;
    user_name_text_edit_box->setPosition(Point(47, 70));
    user_name_text_edit_box->setInputMode(ui::EditBox::InputMode::ANY);
    user_name_text_edit_box->setFontColor(Color4B::WHITE);
    user_name_text_edit_box->setMaxLength(12);
    user_name_text_edit_box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    user_name_text_edit_box->setText(_input_user_name.c_str());
    user_name_text_edit_box->setAnchorPoint(Point(0, 0.5));
    back->addChild(user_name_text_edit_box, 1, TAG_EDITBOX_NAME);
    
    //検索
    Sprite *search = Sprite::create("init/Team/management/search/bt_search.png");
    search->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0));
    search->setScale(ratio);
    this->addChild(search, 1, TAG_SEARCH);
    
}

//タッチイベント処理
bool TeamInvitationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamInvitationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamInvitationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(this->getChildByTag(TAG_SEARCH)==nullptr) return;
    
    //検索
    Sprite *search = (Sprite*)this->getChildByTag(TAG_SEARCH);
    Rect search_rect = search->boundingBox();
    if(search_rect.containsPoint(touch->getLocation())) {
        ui::EditBox *editBox = (ui::EditBox *)this->getChildByTag(TAG_BACK)->getChildByTag(TAG_EDITBOX_ID);
        _input_friend_id = editBox->getText();
        
        editBox = (ui::EditBox *)this->getChildByTag(TAG_BACK)->getChildByTag(TAG_EDITBOX_NAME);
        _input_user_name = editBox->getText();
        
//        editBox = (ui::EditBox *)this->getChildByTag(TAG_EDITBOX_LEVEL_MIN);
//        _input_user_level_min = editBox->getText();
        
//        editBox = (ui::EditBox *)this->getChildByTag(TAG_EDITBOX_LEVEL_MAX);
//        _input_user_level_max = editBox->getText();
        
        string url = HOST;
        url += "team/team_user_search.php";
        url += "?user_master_id=" + SaveData::loadString("user_master_id");
        
        //ユーザID
        url += "&user_id=";
        url += this->_input_friend_id;
        
        //ユーザ名
        url += "&user_name=";
        url += URLEncode(this->_input_user_name);
        
        //ユーザレベル//
//        url += "&user_level_min=";
//        url += this->_input_user_level_min;
//        url += "&user_level_max=";
//        url += this->_input_user_level_max;
        
        SaveData::saveString("team_invitation_friend_id", _input_friend_id);
        SaveData::saveString("team_invitation_user_name", _input_user_name);
//        SaveData::saveString("team_invitation_user_level_min", _input_user_level_min);
//        SaveData::saveString("team_invitation_user_level_max", _input_user_level_max);
        
        //HMSLOG("url=%s", url.c_str());
        auto selector = httpresponse_selector(TeamInvitationLayer::httpCallback);
        HttpConnect::sendHttpRequest(this, url, selector);
    }
}
void TeamInvitationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

//httpコールバック
void TeamInvitationLayer::httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;

    if(result=="") {
        string text = "条件に該当するユーザはいません。";
        popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
        this->addChild(comment, 9999);
    } else {
        SaveData::saveString("team_member_list_mode", "search");
        SaveData::saveString("team_member_data", result);
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("TeamMember");
    }
}
