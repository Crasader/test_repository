#include "TeamManagementBaseLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "Utility.h"
#include "NormalScene.h"
#include "TeamInfoLayer.h"

USING_NS_CC;

#define TAG_INFO 12
#define TAG_INVITATION 13
#define TAG_LEADER 14
#define TAG_WITHDRAWAL 15

//生成
bool TeamManagementBaseLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamManagementBaseLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamManagementBaseLayer::onEnter()
{
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム管理", "TeamTop", this);
    
    Layer::onEnter();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamManagementBaseLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamManagementBaseLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamManagementBaseLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamManagementBaseLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    setup();
}

void TeamManagementBaseLayer::setup()
{
    Vector<MenuItem*> buttons;
        
    //リーダー分岐
    if(SaveData::loadBool("team_leader_flg")==true) {
        //情報変更
        MenuItemSprite *info_change_btn = createMenuItemSprite("init/Team/management/base/bt_info_change.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	info_change_btn->setScale(ratio);
    	info_change_btn->setTag(TAG_INFO);
        buttons.pushBack(info_change_btn) ;
        
        //リーダー引き継ぎ
        MenuItemSprite *leader_change_btn = createMenuItemSprite("init/Team/management/base/bt_leader_change.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	leader_change_btn->setScale(ratio);
    	leader_change_btn->setTag(TAG_LEADER);
        buttons.pushBack(leader_change_btn) ;
        
        //勧誘
        MenuItemSprite *invitation_change_btn = createMenuItemSprite("init/Team/management/base/bt_invitation.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	invitation_change_btn->setScale(ratio);
    	invitation_change_btn->setTag(TAG_INVITATION);
        buttons.pushBack(invitation_change_btn) ;
        
        //脱退
        MenuItemSprite *quit_change_btn = createMenuItemSprite("init/Team/management/base/bt_quit.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	quit_change_btn->setScale(ratio);
    	quit_change_btn->setTag(TAG_WITHDRAWAL);
        buttons.pushBack(quit_change_btn) ;
        
    } else {
        //情報参照
        MenuItemSprite *info_change_btn = createMenuItemSprite("init/Team/management/base/bt_team_info.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	info_change_btn->setScale(ratio);
    	info_change_btn->setTag(TAG_INFO);
        buttons.pushBack(info_change_btn) ;
        
        //勧誘
        MenuItemSprite *invitation_change_btn = createMenuItemSprite("init/Team/management/base/bt_invitation.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    	invitation_change_btn->setScale(ratio);
    	invitation_change_btn->setTag(TAG_INVITATION);
        buttons.pushBack(invitation_change_btn) ;
        
        //脱退
        MenuItemSprite *quit_change_btn = createMenuItemSprite("init/Team/management/base/bt_quit.png", CC_CALLBACK_1(TeamManagementBaseLayer::changePage, this)) ;
    //脱退
    	quit_change_btn->setScale(ratio);
    	quit_change_btn->setTag(TAG_WITHDRAWAL);
        buttons.pushBack(quit_change_btn) ;
    }
    
    Menu* menu = Menu::createWithArray(buttons);
    menu->setPosition(convertFromDisplaySize(Point(320, 430), 0.5, 1));
    menu->alignItemsVertically();
    menu->alignItemsVerticallyWithPadding(10.0f);
    menu->setAnchorPoint(Point(0.5,1));
    this->addChild(menu, 1);
    
}

MenuItemSprite* TeamManagementBaseLayer::createMenuItemSprite(string button_image, const ccMenuCallback& callback)
{
	Sprite* sprite_off = Sprite::create(button_image);			//押されていない時
	Sprite* sprite_on = Sprite::create(button_image);			//押された時
	sprite_on->setColor(Color3B(180,180,180));											//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* change_btn = MenuItemSprite::create(sprite_off, sprite_on, callback);
    return change_btn ;
}


//タッチイベント処理
bool TeamManagementBaseLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamManagementBaseLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TeamManagementBaseLayer::changePage(Ref* sender){
    
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == TAG_INFO)
    {
        normal_scene->changePage("TeamInformationChange");
    }
    else if (tag == TAG_LEADER)
    {
        SaveData::saveString("team_member_list_mode", "leader_change");
        normal_scene->changePage("TeamMember");
    }
    else if (tag == TAG_WITHDRAWAL)
    {
        string text = "本当に脱退してもよろしいですか?";
        popupConfirmLayer *confirm = popupConfirmLayer::create(text);
        auto ok_func = [this](){
            string url = HOST;
            url += "team/team_petition_member.php";
            url += "?user_master_id=" + SaveData::loadString("user_master_id");
            url += "&team_id=" + toString(SaveData::loadInt("team_id"));
            url += "&type=r";
            
            //HMSLOG("url=%s", url.c_str());
            auto selector = httpresponse_selector(TeamManagementBaseLayer::httpCallback);
            HttpConnect::sendHttpRequest(this, url, selector);
            
        };
        auto ng_func = [](){}; //いいえを押したとき何もしない
        confirm->setFunction(ok_func, ng_func);
        this->addChild(confirm, 9999);
    } else if (tag == TAG_INVITATION){
        SaveData::saveString("team_member_list_mode", "search");
//        SaveData::saveString("team_member_data", result);
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("TeamMember");
//        normal_scene->changePage("TeamInvitation");
    }
    
}

void TeamManagementBaseLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
//    NormalScene *normal_scene = (NormalScene*)this->getParent();
//    
//    if(this->getChildByTag(TAG_BACK)==nullptr) return;
//        
//    Sprite *img;
//    Rect rect;
//    
//    //情報変更
//    img = (Sprite*)this->getChildByTag(TAG_INFO);
//    rect = img->boundingBox();
//    if(rect.containsPoint(touch->getLocation())) {
//        normal_scene->changePage("TeamInformationChange");
//        return;
//    }
//    
//    //リーダー引き継ぎ
//    img = (Sprite*)this->getChildByTag(TAG_LEADER);
//    if(img!=nullptr) {
//        rect = img->boundingBox();
//        if(rect.containsPoint(touch->getLocation())) {
//            SaveData::saveString("team_member_list_mode", "leader_change");
//            normal_scene->changePage("TeamMember");
//            return;
//        }
//    }
//    
//    //脱退
//    img = (Sprite*)this->getChildByTag(TAG_WITHDRAWAL);
//    rect = img->boundingBox();
//    if(rect.containsPoint(touch->getLocation())) {
//        string text = "本当に脱退してもよろしいですか?";
//        popupConfirmLayer *confirm = popupConfirmLayer::create(text);
//        auto ok_func = [this](){
//            string url = HOST;
//            url += "team/team_petition_member.php";
//            url += "?user_master_id=" + SaveData::loadString("user_master_id");
//            url += "&team_id=" + toString(SaveData::loadInt("team_id"));
//            url += "&type=r";
//            
//            //HMSLOG("url=%s", url.c_str());
//            auto selector = httpresponse_selector(TeamManagementBaseLayer::httpCallback);
//            HttpConnect::sendHttpRequest(this, url, selector);
//            
//        };
//        auto ng_func = [](){}; //いいえを押したとき何もしない
//        confirm->setFunction(ok_func, ng_func);
//        
//        this->addChild(confirm, 9999);
//        
//        return;
//    }
//    
//    //勧誘
//    img = (Sprite*)this->getChildByTag(TAG_INVITATION);
//    rect = img->boundingBox();
//    if(rect.containsPoint(touch->getLocation())) {
//        normal_scene->changePage("TeamInvitation");
//        return;
//    }
//    
//    //戻る
//    Sprite *back = (Sprite*)this->getChildByTag(TAG_BACK);
//    Rect back_rect = back->boundingBox();
//    if(back_rect.containsPoint(touch->getLocation())) {
//        normal_scene->changePage("TeamTop");
//    }

}
void TeamManagementBaseLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TeamManagementBaseLayer::httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamNoBelong");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

