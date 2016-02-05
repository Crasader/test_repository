#include "popupConfirmLayer.h"
#include "ReplayScene.h"

popupConfirmLayer* popupConfirmLayer::create(string message, PopupSize popup_size)
{
    popupConfirmLayer *node = new popupConfirmLayer();
    if (node && node->init(message, popup_size))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool popupConfirmLayer::init(string message, PopupSize popup_size)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    _popup_size = popup_size;
    
    return true;
}

void popupConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ポップアップ画像の表示
    showImage();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void popupConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void popupConfirmLayer::showImage()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    string path = "init/Home/Popup/popup_300.png";;
    switch (_popup_size)
    {
        case Popup_400: path = "init/Home/Popup/popup_400.png"; break;
        case Popup_500: path = "init/Home/Popup/popup_500.png"; break;
        default: break;
    }
    
    //背景
    _bg = Sprite::create(path);
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 + 30));
    _bg->addChild(label, Z_Message, T_Message);
    
    //はいボタン作成
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(150, 150, 150));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [&](Ref* sender)
    {
        //タップ音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        
        if(_ok_function != nullptr) _ok_function();
        
        //閉じる
        if(_for_replay_flg==false) Utility::closePopupAction(this);
    });
    yes_btn->setTag(T_Yes_MenuItem);
    
    //いいえボタン作成
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(150, 150, 150));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
        //タップ音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        
        if(_ng_function != nullptr) _ng_function();
        
        //閉じる
        Utility::closePopupAction(this);
    });
    no_btn->setTag(T_No_MenuItem);
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu, Z_Menu, T_Menu);
}

void popupConfirmLayer::setFunction(function<void()> ok_func, function<void()> ng_func)
{
    _ok_function = ok_func;
    _ng_function = ng_func;
}

void popupConfirmLayer::setFavoriteFlg(bool favorite_flg, string battle_list_id)
{
    _favorite_flg = favorite_flg;
    _battle_list_id = battle_list_id;
    _for_replay_flg = true;
}

void popupConfirmLayer::favoriteRequest()
{
    //リプレイお気に入り通信処理
    string user_master_id    = SaveData::loadString("user_master_id");
    string to_user_master_id = SaveData::loadString("to_user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    if(_favorite_flg==true) {
        url += "battle/delete_replay_list.php";
    } else {
        url += "battle/insert_replay_list.php";
    }
    auto selector = httpresponse_selector(popupConfirmLayer::favoriteRequestCallback);
    string body = "user_master_id=" + user_master_id + "&battle_list_id=" + _battle_list_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void popupConfirmLayer::favoriteRequestCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    auto replay_scene = (ReplayScene *)this->getParent();
    
    string message = result;
    auto popup_layer = popupNoChangePageLayer::create(message);
    replay_scene->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    
    this->removeFromParent();
}

bool popupConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("popupConfirmLayer touchBegan!");
    return true;
}
void popupConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
