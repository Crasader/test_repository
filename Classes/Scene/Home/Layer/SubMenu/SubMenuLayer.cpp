#include "SubMenuLayer.h"

bool SubMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("SubMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void SubMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("サブメニュー", "Mypage", this);
    
    //ボタンとスクロールの作成
    createButton();
}

void SubMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SubMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SubMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SubMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(SubMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void SubMenuLayer::createButton()
{
    string array[] = {
        "お問い合わせ",
        "ヘルプ",
        "ランキング",
        "プロフィール",
        "公式ホームページ",
#if (IF_ANDROID)
        "シリアル入力",
#endif
        "タイトルに戻る",
        "その他"
    };
    
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++)
    {
        //ボタン画像
        _button_list.push_back(Sprite::create("init/Home/Common/bt_empty.png"));
        _button_list[i]->setTag(i + 1);
        
        //ボタン文字
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(array[i], FONT_NAME_SYSTEM, FONT_SIZE_LIST);
        label->setPosition(Point(_button_list[i]->getContentSize() / 2));
        _button_list[i]->addChild(label);
    }
    
    auto func = [this](int tag){
        changePage(tag);
    };
    
    //スクロールレイヤーの作成
    auto scroll_layer = HmsScrollLayer::create(130 * ratio, _button_list, func);
    this->addChild(scroll_layer, 1, SCROLLLAYER_TAG);
}

void SubMenuLayer::changePage(int tag)
{
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == T_Profile) {
        //プロフィールへ
        normal_scene->changePage("Profile");
    }
#if (IF_ANDROID)
    else if (tag == T_Serial) {
        //シリアル入力へ
        string url = HOST;
        url += "user/index.php?&p=page__serial_input&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "シリアル入力");
        normal_scene->addChild(WebViewLoadLayer, 10);
    }
#endif
    else if (tag == T_Inquiry) {
        //問い合わせへ
        string url = HOST;
        url += "user/index.php?&p=page__inquiry&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "問い合わせ");
        normal_scene->addChild(WebViewLoadLayer, 10);
    }else if(tag == T_Help){//ヘルプ
        string url = HOST;
        url += "user/?&p=page__help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 10);
    }else if (tag == T_Homepage) {
        //ホームページへ
        string url = "http://www.hmsystems.com/3bw/";
        NativeBridge::openURL(url.c_str());
    }
    else if (tag == T_Other) {
        //その他へ
        normal_scene->changePage("Other");
    }
    else if (tag == T_Title) {
        //タイトルへ
        auto scene = TitleScene::createScene();
        auto transition = TransitionFade::create(3.0f, scene);
        Director::getInstance()->replaceScene(transition);
	}else if (tag == T_Ranking) {
		//ランキングへ
		string url = HOST;
        url += "webview/ranking.php?type=0";
        url += "&user_master_id=" + SaveData::loadString("user_master_id");
		auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ランキング");
		normal_scene->addChild(WebViewLoadLayer, 10);
	}
	
}

bool SubMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void SubMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{

}
void SubMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void SubMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{

}
