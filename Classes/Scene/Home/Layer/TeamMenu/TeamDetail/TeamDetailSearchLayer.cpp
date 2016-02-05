#include "TeamDetailSearchLayer.h"
#include "HmsGeometricFunction.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

TeamDetailSearchLayer* TeamDetailSearchLayer::create(json11::Json json)
{
    TeamDetailSearchLayer *sprite = new TeamDetailSearchLayer();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void TeamDetailSearchLayer::createButton()
{
    //申請
    auto app_btn_on = Sprite::create("init/Team/no_belong/team_search/bt_apply.png");
    auto app_btn_off = Sprite::create("init/Team/no_belong/team_search/bt_apply.png");
    app_btn_on->setColor(Color3B(180, 180, 180));
    auto app_btn = MenuItemSprite::create(app_btn_off, app_btn_on, [this](Ref* sender){
        this->buttonTapAction();
    });
    auto app_menu = Menu::create(app_btn, nullptr);
    app_menu->setPosition(Point(frame->getContentSize().width/2, -50));
    frame->addChild(app_menu);
}

void TeamDetailSearchLayer::buttonTapAction()
{
    string url = HOST;
    url += "team/team_petition_member.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&team_id=" + teamData["team_id"].string_value();
    url += "&type=a";
    
    HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamDetailSearchLayer::buttonTapCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamDetailSearchLayer::buttonTapCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->getParent()->addChild(layer, 9999);
        
        refreshList();
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->getParent()->addChild(layer, 9999);
    }
}