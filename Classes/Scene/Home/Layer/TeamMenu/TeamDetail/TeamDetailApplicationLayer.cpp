#include "TeamDetailApplicationLayer.h"
#include "HmsGeometricFunction.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

TeamDetailApplicationLayer* TeamDetailApplicationLayer::create(json11::Json json)
{
    TeamDetailApplicationLayer *sprite = new TeamDetailApplicationLayer();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void TeamDetailApplicationLayer::createButton()
{
    //申請キャンセル
    auto cancel_btn_on = Sprite::create("init/Team/no_belong/team_search/bt_cancel.png");
    auto cancel_btn_off = Sprite::create("init/Team/no_belong/team_search/bt_cancel.png");
    cancel_btn_on->setColor(Color3B(180, 180, 180));
    auto cancel_btn = MenuItemSprite::create(cancel_btn_off, cancel_btn_on, [this](Ref* sender){
        this->buttonTapAction();
    });
    auto cancel_menu = Menu::create(cancel_btn, nullptr);
    cancel_menu->setPosition(Point(frame->getContentSize().width/2, -50));
    frame->addChild(cancel_menu);
}

void TeamDetailApplicationLayer::buttonTapAction()
{
        string url = HOST;
        url += "team/team_petition_member.php";
        url += "?user_master_id=" + SaveData::loadString("user_master_id");
        url += "&team_id=" + teamData["team_id"].string_value();
        url += "&type=d";
        
        HMSLOG("url=%s", url.c_str());
        auto selector = httpresponse_selector(TeamDetailApplicationLayer::buttonTapCallback);
        HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamDetailApplicationLayer::buttonTapCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
        this->addChild(layer, 9999);
    }
}
