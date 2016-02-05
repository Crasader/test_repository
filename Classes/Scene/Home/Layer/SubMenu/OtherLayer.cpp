#include "OtherLayer.h"

bool OtherLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("OtherLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    initEventListener();
    
    return true;
}

void OtherLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void OtherLayer::setupImage(){
    
    //取得した音量の適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(SaveData::loadFloat("bgm_volume"));
    //取得した音量の適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(SaveData::loadFloat("effect_volume"));
    
    header_height = 140.0f;//ヘッダーの高さ
    footer_height = 108.0f;//フッターの高さ
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height); //ヘッダーフッター以外の高さ
    main_screen_size_w = (visibleSize.width  / ratio); //ヘッダーフッター以外の高さ
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("その他", "SubMenu", this);
    //オプション画面遷移ボタン
    auto options = Sprite::create("init/Home/Common/bt_empty.png");
    options->setScale(ratio);
    options->setTag(OPTIONS_CHANGE_TAG);
    options->setPosition(convertFromDisplaySize(Point(320, 620), 0.5, 1));
    this->addChild(options,8);
    
    auto optionslabel = HMSFontUtil::createWithTTFBySystemFontArgument("オプション", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
    optionslabel->setScale(ratio);
    optionslabel->setPosition(convertFromDisplaySize(Point(320, 620), 0.5, 1));
    optionslabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(optionslabel, 10);
    
    //機種変更コード入力画面遷移ボタン
    auto modelchange = Sprite::create("init/Home/Common/bt_empty.png");
    modelchange->setScale(ratio);
    modelchange->setTag(MODELCHANGE_CHANGE_TAG);
    modelchange->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 1));
    this->addChild(modelchange,8);
    
    auto modelchangecodelabel = HMSFontUtil::createWithTTFBySystemFontArgument("機種変更", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
    modelchangecodelabel->setScale(ratio);
    modelchangecodelabel->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 1));
    modelchangecodelabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(modelchangecodelabel, 10);
    
    //クレジット画面遷移ボタン
    auto Credit = Sprite::create("init/Home/Common/bt_empty.png");
    Credit->setScale(ratio);
    Credit->setTag(CREDIT_TAG);
    Credit->setPosition(convertFromDisplaySize(Point(320,340), 0.5, 1));
    this->addChild(Credit,8);
    
    auto creditlabel = HMSFontUtil::createWithTTFBySystemFontArgument("クレジット", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
    creditlabel->setScale(ratio);
    creditlabel->setPosition(convertFromDisplaySize(Point(320,340), 0.5, 1));
    creditlabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(creditlabel, 10);
}

void OtherLayer::changePage(int tag_no){
    if(tag_no == OPTIONS_CHANGE_TAG){//オプション
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Sound");
        HMSLOG("touch Sound!");
    }else    
    if(tag_no == MODELCHANGE_CHANGE_TAG){//機種変更
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Modelchange");
        HMSLOG("touch Modelchange!");
    }else
    if(tag_no == CREDIT_TAG){//クレジット
        //webView使用
        auto normal_scene = (NormalScene *)this->getParent();
        string url = CREDIT_URL"&user_id="+user_master_id;
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "クレジット");
        normal_scene->addChild(WebViewLoadLayer,9);
    }else
    if(tag_no == BACK_BUTTON_TAG){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Mypage");
        HMSLOG("touch back!");
    }
}

void OtherLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(OtherLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(OtherLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(OtherLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(OtherLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool OtherLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    first_touch = touch->getLocation();
    HMSLOG("onTouchBegan");
    return true;
}
void OtherLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
//    move_flg = true;
}
void OtherLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto children = this->getChildren();
//    if(!move_flg){//スワイプしていないとき
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                int tag = sp->getTag();
                HMSLOG("tag = %d", tag);
                changePage(tag);
            }
        }
//    }else{//スワイプしているとき
//        move_flg = false;//スワイプ終了判定
//    }
    HMSLOG("onTouchEnded");
}
void OtherLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchCancelled");
}
