#include "BoxGachaLayer.h"
#include "BoxGachaConfirmLayer.h"

bool BoxGachaLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BoxGachaLayer::init()");
    
    return true;
}

void BoxGachaLayer::onEnter()
{
    ratio = getVisibleRatio();
    
    Layer::onEnter();
    
    _box_gacha_id = SaveData::loadString("box_gacha_id");
    
    //通信してメンテ状態を確認する
    gachaRequest();
}

void BoxGachaLayer::maintenanceRequest()
{
    //メンテ・課金可能確認
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "user/pay_check.php";
    auto selector = httpresponse_selector(BoxGachaLayer::maintenanceCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BoxGachaLayer::maintenanceCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        gachaRequest();
    }
}

void BoxGachaLayer::gachaRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "user/box_gacha_data.php";
    url += "?user_master_id="+user_master_id;
    url += "&resource_version=" + SaveData::loadString("resource_version");
    url += "&box_gacha_id=" + _box_gacha_id;
    auto selector = httpresponse_selector(BoxGachaLayer::gachaCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void BoxGachaLayer::gachaCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        
        string err;
        _json = Json::parse(result, err);
        
        //ダウンロードする必要なデータがあった時タイトルに戻す処理
        if(_json["needs_download"].bool_value()){
            auto normal_scene = (NormalScene *)this->getParent();
            auto func = [](){
                //タイトルへ
                auto scene = TitleScene::createScene();
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
            };
            //ボップアップ表示
            string message = _json["message"].string_value() ;
            auto popup_layer = PopupOkButtonLayer::create(func, message, false);
            normal_scene->addChild(popup_layer, 999999);
            return ;
        }
        
        //データに不整合があった時ガチャメニューに戻す
        if(_json["illegal_flg"].bool_value()){
            auto normal_scene = (NormalScene *)this->getParent();
            auto func = [normal_scene](){
                normal_scene->changePage("RevisionGachaMenu");
            };
            //ボップアップ表示
            string message = _json["message"].string_value() ;
            auto popup_layer = PopupOkButtonLayer::create(func, message, false);
            normal_scene->addChild(popup_layer, 999999);
            return ;
        }
        
        _box_height = _json["box_height"].int_value();
        int box_gacha_ticket_count = _json["box_gacha_ticket_count"].int_value();
        SaveData::saveInt("box_gacha_ticket_count", box_gacha_ticket_count);
        
        //セットアップ
        initEventListener();
        
        setupButton();
        
        createScrollView();
    }
}

void BoxGachaLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BoxGachaLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BoxGachaLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BoxGachaLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BoxGachaLayer::setupButton()
{
    //タイトル
    HeaderLayer::setupTitleBar("BOXガチャ", "RevisionGachaMenu", this);
    
    Sprite *img;
    //背景
    img = Sprite::create("init/Home/Gacha/bk_gacha_box.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 425), 0.5, 0.5));
    this->addChild(img);
    
    //ガチャボタン
    string folder_name = "download/Gacha/box_gacha/" + _box_gacha_id + "/";
    string gray;
    //1連
    if(_json["bt_1"].bool_value()==true) gray = "";
    else gray = "_gray";
    img = Sprite::create(folder_name + "bt_1" + gray + ".png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(453, 307), 0.5, 0.5));
    this->addChild(img, 3, TAG_BOX_GACHA_BTN_JEWEL1);
    
    //10連
    if(_json["bt_10"].bool_value()==true) gray = "";
    else gray = "_gray";
    img = Sprite::create(folder_name + "bt_10" + gray + ".png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(187, 180), 0.5, 0.5));
    this->addChild(img, 3, TAG_BOX_GACHA_BTN_JEWEL10);
    
    //30連
    if(_json["bt_30"].bool_value()==true) gray = "";
    else gray = "_gray";
    img = Sprite::create(folder_name + "bt_30" + gray + ".png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(453, 180), 0.5, 0.5));
    this->addChild(img, 3, TAG_BOX_GACHA_BTN_JEWEL30);
    
    //チケット1
    if(_json["bt_1t"].bool_value()==true) gray = "";
    else gray = "_gray";
    img = Sprite::create(folder_name + "bt_1t" + gray + ".png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(187, 307), 0.5, 0.5));
    this->addChild(img, 3, TAG_BOX_GACHA_BTN_TICKET1);
    
    //確率ボタン
    img = Sprite::create("init/Home/Gacha/bt_probability_box.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(550, 782), 0.5, 1));
    this->addChild(img, 3, TAG_BOX_GACHA_RATE);
}

void BoxGachaLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, 266+FOOTER_SIZE), 0.5, 0.5));
    pScrollView->setViewSize(Size(640 * ratio, 316 * ratio));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 1, SCROLLVIEW_TAG);
    
    //スクロールするレイヤーを作成する
    auto layer = createLayer();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _box_height*ratio));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    //pScrollView->setContentOffset(Point(0, 0 - (_box_height - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
    pScrollView->setContentOffset(Point(0, 0));
    //pScrollView->setContentOffset(Point(0, -_box_height*ratio));
    
    //レイヤーアニメーション
    DelayTime *delay = DelayTime::create(1.0f);
    MoveBy *move = MoveBy::create(5.0f, Point(0, -(_box_height-316)*ratio));
    layer->runAction(Sequence::create(delay, move, nullptr));
}

Layer *BoxGachaLayer::createLayer()
{
    Layer *layer = Layer::create();

    Sprite *img;
    
    /*
    //背景
    img = Sprite::create("init/Home/Common/back_shadow50.png");
    img->setScale(ratio);
    img->setAnchorPoint(Point(0.5, 0.5));
    img->setPosition(Point(320, 0));
    layer->addChild(img);
     */
    
    //中身
    int data_count = _json["data_count"].int_value();

    /*
    Sprite *img = Sprite::create("init/CharaMenu/arrow_left.png");
    img->setScale(ratio);
    img->setPosition(Point(320*ratio, 0));
    layer->addChild(img);
     */
    
    for(int i=1; i<=data_count; i++) {
        string iStr = toString(i);
        int now_count = _json["list"+iStr]["now_count"].int_value();
        int max_count = _json["list"+iStr]["max_count"].int_value();
        string rarity = _json["list"+iStr]["rarity"].string_value();
        string chara_id1 = _json["list"+iStr]["chara_id1"].string_value();
        string chara_id2 = _json["list"+iStr]["chara_id2"].string_value();
        string chara_id3 = _json["list"+iStr]["chara_id3"].string_value();
        
        HMSLOG("rarity=%s,now_count=%d,max_count=%d", rarity.c_str(), now_count, max_count);
        
        int y = (data_count-i)*100+50;
        
        //背景
        img = Sprite::create("init/Home/Gacha/bk_gacha_box_info.png");
        img->setScale(ratio);
        img->setAnchorPoint(Point(0.5, 0.5));
        img->setPosition(Point(320*ratio, y*ratio));
        layer->addChild(img);
        
        //枚数
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(toString(now_count)+"/"+toString(max_count)+"枚", FONT_NAME_SYSTEM, 22*ratio);
        //label->setHorizontalAlignment(TextHAlignment::LEFT);
        label->setPosition(Point(545*ratio, (y+11)*ratio));
        label->setAnchorPoint(Point(1, 0.5));
        layer->addChild(label);
        
        //ゲージ
        img = Sprite::create("init/Home/Gacha/gacha_box_info_bk.png");
        img->setScale(ratio);
        img->setAnchorPoint(Point(0, 0.5));
        img->setPosition(Point(405*ratio, (y-26)*ratio));
        layer->addChild(img);
        
        float percentage = (float)now_count / (float)max_count;
        img = Sprite::create("init/Home/Gacha/gacha_box_info_green.png");
        img->setScale(ratio);
        img->setScaleX(ratio*percentage);
        img->setAnchorPoint(Point(0, 0.5));
        img->setPosition(Point(414*ratio, (y-26)*ratio));
        layer->addChild(img);
        
        //レアリティ表示
        if(rarity!="") {
            img = Sprite::create("init/Home/Gacha/rare_gacha_box" + rarity + ".png");
            img->setScale(ratio);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(Point(120*ratio, y*ratio));
            layer->addChild(img);
        } else {
            img = Sprite::create("init/Home/Gacha/gacha_box_featured.png");
            img->setScale(ratio);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(Point(120*ratio, y*ratio));
            layer->addChild(img);
        }

        //目玉キャラ表示
        if(chara_id1!="") {
            img = Sprite::create("download/Chara/thumb100/" + toString(chara_id1) + ".png");
            img->setScale(ratio*0.7);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(Point(198*ratio, y*ratio));
            layer->addChild(img);
        }
        if(chara_id2!="") {
            img = Sprite::create("download/Chara/thumb100/" + toString(chara_id2) + ".png");
            img->setScale(ratio*0.7);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(Point(276*ratio, y*ratio));
            layer->addChild(img);
        }
        if(chara_id3!="") {
            img = Sprite::create("download/Chara/thumb100/" + toString(chara_id3) + ".png");
            img->setScale(ratio*0.7);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(Point(353*ratio, y*ratio));
            layer->addChild(img);
        }

    }
    
    return layer;
}

bool BoxGachaLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Sprite *img;
    Rect rect;
    img = (Sprite *)this->getChildByTag(TAG_BOX_GACHA_BTN_JEWEL1);
    rect = img->getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        HMSLOG("confirm jewel1");
        
        if(_json["bt_1"].bool_value()==false) return true;
        
        auto normal_scene = (NormalScene *)this->getParent();
        auto confirm_layer = BoxGachaConfirmLayer::create(TAG_BOX_GACHA_BTN_JEWEL1, _box_gacha_id);//Layer呼び出し
        confirm_layer->setScale(0*ratio);
        confirm_layer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
        normal_scene->addChild(confirm_layer, ZORDER_SHADOW+1);
        
        return true;
    }
    img = (Sprite *)this->getChildByTag(TAG_BOX_GACHA_BTN_JEWEL10);
    rect = img->getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        HMSLOG("confirm jewel10");
        
        if(_json["bt_10"].bool_value()==false) return true;

        auto normal_scene = (NormalScene *)this->getParent();
        auto confirm_layer = BoxGachaConfirmLayer::create(TAG_BOX_GACHA_BTN_JEWEL10, _box_gacha_id);//Layer呼び出し
        confirm_layer->setScale(0*ratio);
        confirm_layer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
        normal_scene->addChild(confirm_layer, ZORDER_SHADOW+1);
        
        return true;
    }
    img = (Sprite *)this->getChildByTag(TAG_BOX_GACHA_BTN_JEWEL30);
    rect = img->getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        HMSLOG("confirm jewel30");
        
        if(_json["bt_30"].bool_value()==false) return true;
        
        auto normal_scene = (NormalScene *)this->getParent();
        auto confirm_layer = BoxGachaConfirmLayer::create(TAG_BOX_GACHA_BTN_JEWEL30, _box_gacha_id);//Layer呼び出し
        confirm_layer->setScale(0*ratio);
        confirm_layer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
        normal_scene->addChild(confirm_layer, ZORDER_SHADOW+1);
        
        return true;
    }
    img = (Sprite *)this->getChildByTag(TAG_BOX_GACHA_BTN_TICKET1);
    rect = img->getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        HMSLOG("confirm ticket1");
        
        if(_json["bt_1t"].bool_value()==false) return true;
        
        auto normal_scene = (NormalScene *)this->getParent();
        auto confirm_layer = BoxGachaConfirmLayer::create(TAG_BOX_GACHA_BTN_TICKET1, _box_gacha_id);//Layer呼び出し
        confirm_layer->setScale(0*ratio);
        confirm_layer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
        normal_scene->addChild(confirm_layer, ZORDER_SHADOW+1);
        
        return true;
    }
    
    img = (Sprite *)this->getChildByTag(TAG_BOX_GACHA_RATE);
    rect = img->getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        HMSLOG("gacha rate");
        
        auto normal_scene = (NormalScene *)this->getParent();
        
        string url = HOST;
        url += "user/index.php?p=page__box_gacha_rate&box_gacha_id=" + _box_gacha_id + "&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "キャラクター出現確率");
        normal_scene->addChild(WebViewLoadLayer, 10);
        
        return true;
    }

    return true;
}


void BoxGachaLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void BoxGachaLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

