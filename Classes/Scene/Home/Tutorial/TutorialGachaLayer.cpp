#include "TutorialGachaLayer.h"

bool TutorialGachaLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TutorialGachaLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ボタンタッチフラグの初期設定
    _touchFlg = false;
    
    return true;
}

void TutorialGachaLayer::createMessageBg()
{
    //文字の背景画像
    message_bg = MessageSprite::create("初回に10連ガチャを一回\n引くことができるよ。", TextHAlignment::LEFT);
    message_bg->setScale(ratio);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 660), 0.5, 0.5));
    addChild(message_bg, 1, MESSAGE_SPRITE_TAG);
}

void TutorialGachaLayer::createBunner()
{
    //バナー画像の作成
    auto banner = Sprite::create("download/Gacha/banner_gacha_detail1.png");
    banner->setScale(ratio);
    banner->setPosition(convertFromDisplaySize(Point(320, 380), 0.5, 0.5));
    addChild(banner, 1);
}

void TutorialGachaLayer::createButton()
{
    //初回10連ガチャを引くボタン
    auto btn_off = Sprite::create("init/Home/Common/bt1.png");
    auto btn_on  = Sprite::create("init/Home/Common/bt1.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto btn_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("10回連続", FONT_NAME_SYSTEM, 28);
    btn_label1->setPosition(btn_off->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    btn_off->addChild(btn_label1, 1);
    
    auto btn_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("10回連続", FONT_NAME_SYSTEM, 28);
    btn_label2->setPosition(btn_off->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    btn_on->addChild(btn_label2, 1);
    
    auto btn = MenuItemSprite::create(btn_off, btn_on, [this](Ref* sender)
    {
        //10連ガチャボタン処理
        if (!_touchFlg)
        {
            _touchFlg = true;
            
            //チュートリアルステータスの変更
            SaveData::saveInt("tutorialStatus", 3);
            
            //通信処理開始
            tutorialGachaRequest();
        }
    });
    
    btn->setScale(1.2 * ratio);
    auto Menu = Menu::create(btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(320, 130), 0.5, 0.5));
    addChild(Menu, 1);
}

void TutorialGachaLayer::tutorialGachaRequest()
{
    //10連ガチャリクエスト
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "tutorial/tutorial_gacha.php";
    auto selector = httpresponse_selector(TutorialGachaLayer::tutorialGachaCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TutorialGachaLayer::tutorialGachaCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s", result.c_str());
    
    SaveData::saveString("tutorial_gacha", result);
    
    //デッキ更新リクエスト
    charaDeckUpdateRequest();
    
}

void TutorialGachaLayer::charaDeckUpdateRequest()
{
    //デッキ更新リクエスト
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "tutorial/tutorial_chara_deck_update.php";
    auto selector = httpresponse_selector(TutorialGachaLayer::charaDeckUpdateCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TutorialGachaLayer::charaDeckUpdateCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s", result.c_str());

    //ページ遷移
    auto tutorial_scene = (TutorialScene *)this->getParent();
    auto next_layer = TutorialGachaAnimationLayer::create();
    tutorial_scene->changePage(next_layer);
}

void TutorialGachaLayer::onEnter()
{
    Layer::onEnter();
    
    //メッセージ画像の背景作成
    createMessageBg();
    
    //ガチャバナーの作成
    createBunner();
    
    //ボタンの作成
    createButton();
}
