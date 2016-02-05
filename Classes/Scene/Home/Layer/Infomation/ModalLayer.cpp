#include "ModalLayer.h"



ModalLayer* ModalLayer::create(int contents)
{
    return ModalLayer::create(contents, nullptr, nullptr, 0) ;
}

ModalLayer* ModalLayer::create(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime)
{
    auto p = new ModalLayer();
    if (p->init(contents, parent, sel, schedulerTime)) {
        p->autorelease();
        return p;
    } else {
        delete p;
        return nullptr;
    }
}

bool ModalLayer::init(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime)
{
	if ( !Layer::init() )
	{
		return false;
	}
    HMSLOG("ModalLayer init()");
    
	ratio = getVisibleRatio();
		
	//表示するコンテンツ番号の取得
    _contents = contents;
    
    setScheduler(parent, sel, schedulerTime) ;
    
    return true;
}

void ModalLayer::setScheduler(Layer *parent, SEL_SCHEDULE sel, int schedulerTime)
{
    _parent = parent;
    _sel = sel;
    _schedulerTime = schedulerTime ;
}

void ModalLayer::createContentsData()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);
	//フレーム内の構築
    if(_contents == MODAL_CONTENTS_CHARA || _contents == MODAL_CONTENTS_POWER || _contents == MODAL_CONTENTS_EVENT_POWER || _contents == MODAL_CONTENTS_BOSS_EVENT_POWER)
    {
		//ボタン画像の余白部分をのぞいたときのサイズ
		int bt_visual_size_y = 70 ;
        
		//テキストの中心位置(フレームの下端を原点とするときの) = ボタンの上端とフレームの上端の中心 = 二点の座標の合計/2
		float text_position_y = (_frame->getContentSize().height + (pOkBtn->getPosition().y + bt_visual_size_y/2)) / 2 ;
		
		//通信失敗処理
		string text = "" ;
		if(_contents==MODAL_CONTENTS_CHARA){
			text += "最大所持数を10追加しますか？\n(宝玉1個)" ;
		}else if(_contents==MODAL_CONTENTS_POWER){
			text += "行動力を全回復しますか？\n(宝玉1個)" ;
		}else if(_contents==MODAL_CONTENTS_EVENT_POWER){
			text += "イベント行動力を全回復しますか？\n(宝玉1個)" ;
        }else if(_contents==MODAL_CONTENTS_BOSS_EVENT_POWER){
            text += "イベント行動力を全回復しますか？\n(宝玉1個)" ;
        }
		_message = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
		_message->setHorizontalAlignment(TextHAlignment::CENTER);
		_message->setPosition(Point(_frame->getContentSize().width/2, text_position_y));
//enableOutline(Color4B(0, 0, 0, 255), 3);
		_message->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
		_frame->addChild(_message);
		
	}
    else if(_contents == MODAL_CONTENTS_FRIEND) {

//		//LINEボタン
//		MenuItemImage *pLineBtn = MenuItemImage::create("init/Friend/bt_line.png", "init/Friend/bt_line.png", [&](Ref* pSender){
//			bool bResult = NativeBridge::openURL("http://graphic-loops.com");
//			if (bResult == false) {
//				HMSLOG("openURL Error...");
//			}
//			else {
//				HMSLOG("openURL Done!!");
//			}
//		});
//        
//		//Twitterボタン
//		MenuItemImage *pTwitterBtn = MenuItemImage::create("init/Friend/bt_twitter.jpg", "init/Friend/bt_twitter.jpg", [&](Ref* pSender){
//						bool bResult = NativeBridge::openURL("http://graphic-loops.com");
//						if (bResult == false) {
//							HMSLOG("openURL Error...");
//						}
//						else {
//							HMSLOG("openURL Done!!");
//						}
//						
//			//WebView構築
//			string url = HOST;
//			url += "user/index.php?p=page__gacha_rate";
//			pWebView = experimental::ui::WebView::create();
//			pWebView->setAnchorPoint(Point(0.0f, 1.0f));
//			pWebView->setContentSize(Size(_frame->getContentSize().width-20, _frame->getContentSize().height-20));
//			pWebView->setPosition(Point(10, _frame->getContentSize().height-10));
//			//            pWebView->loadURL("http://www.kairisei-ma.jp");
//			pWebView->loadURL(url);
//			pWebView->setScalesPageToFit(true);
//			_frame->addChild(pWebView);
//		});
	}
    else if (_contents == MODAL_CONTENTS_GACHARATE) {
		//インデックス画像
		auto rateIndex = Sprite::create("init/Home/Popup/popup_index_s.png");
		rateIndex->setAnchorPoint(Point(0.5, 0));
		float index_width = _frame->getContentSize().width / 2;
		float index_height = _frame->getContentSize().height - 5;
		rateIndex->setPosition(Point(index_width, index_height));
		
		auto rateIndexLabel = HMSFontUtil::createWithTTFBySystemFontArgument("キャラクター出現確率", FONT_NAME_SYSTEM, 30);
		rateIndexLabel->setPosition(rateIndex->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
		rateIndex->addChild(rateIndexLabel);
		
		_frame->addChild(rateIndex, 1);
		
		
		//クローズボタン
		MenuItemImage* pCloseItem = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close.png", [&](Ref* pSender){
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
			this->removeFromParentAndCleanup(true);
		});
		pCloseItem->setScale(0.6);
		
		Menu* pMenu = Menu::create(pCloseItem, NULL);
		float close_width = rateIndex->getContentSize().width - 35;
		float close_height = rateIndex->getContentSize().height / 2;
		pMenu->setPosition(Point(close_width, close_height));
		rateIndex->addChild(pMenu);
		
		//WebView構築
		string url = HOST;
		url += "user/index.php?p=page__gacha_rate";
		pWebView = experimental::ui::WebView::create();
		pWebView->setAnchorPoint(Point(0.0f, 1.0f));
		pWebView->setContentSize(Size(_frame->getContentSize().width-20, _frame->getContentSize().height-20));
		pWebView->setPosition(Point(10, _frame->getContentSize().height-10));
		pWebView->loadURL(url);
		pWebView->setScalesPageToFit(true);
		_frame->addChild(pWebView, 1);
	}
    else if (_contents == MODAL_CONTENTS_SERIAL) {
		//インデックス画像
		auto rateIndex = Sprite::create("init/Home/Popup/popup_index_s.png");
		//        rateIndex->setScale(ratio);
		rateIndex->setAnchorPoint(Point(0.5, 0));
		float index_width = _frame->getContentSize().width / 2;
		float index_height = _frame->getContentSize().height - 5;
		rateIndex->setPosition(Point(index_width, index_height));
		
		auto rateIndexLabel = HMSFontUtil::createWithTTFBySystemFontArgument("キャラクター出現確率", FONT_NAME_SYSTEM, 30);
		rateIndexLabel->setPosition(rateIndex->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
		rateIndex->addChild(rateIndexLabel);
		
		_frame->addChild(rateIndex, 1);
		
		
		//クローズボタン
		MenuItemImage* pCloseItem = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close.png", [&](Ref* pSender){
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
			this->removeFromParentAndCleanup(true);
		});
		pCloseItem->setScale(0.6);
		
		Menu* pMenu = Menu::create(pCloseItem, NULL);
		float close_width = rateIndex->getContentSize().width - 35;
		float close_height = rateIndex->getContentSize().height / 2;
		pMenu->setPosition(Point(close_width, close_height));
		rateIndex->addChild(pMenu);
		
		//WebView構築
		string url = HOST;
		url += "user/index.php?p=page__gacha_rate";
		pWebView = experimental::ui::WebView::create();
		pWebView->setAnchorPoint(Point(0.0f, 1.0f));
		pWebView->setContentSize(Size(_frame->getContentSize().width-20, _frame->getContentSize().height-20));
		pWebView->setPosition(Point(10, _frame->getContentSize().height-10));
		pWebView->loadURL(url);
		pWebView->setScalesPageToFit(true);
		_frame->addChild(pWebView, 1);
	} else if(MODAL_CONTENTS_TERMS) {
		//同意勧告
		string agreement_txt = "当アプリで遊ぶためには\n利用規約に同意する必要があります。\n利用規約に同意しますか？";
		auto agreement_label = HMSFontUtil::createWithTTFBySystemFontArgument(agreement_txt.c_str(),FONT_NAME_SYSTEM,30);
		agreement_label->setHorizontalAlignment(TextHAlignment::CENTER);
		agreement_label->setAnchorPoint(Point(0.5,1));
		agreement_label->enableOutline(Color4B::BLACK,1);
		agreement_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
		agreement_label->setPosition(Point(_frame->getContentSize().width / 2, _frame->getContentSize().height - 50));
		_frame->addChild(agreement_label,4);
		
		
		//利用規約ボタン
		Sprite* agreement_btn_off = Sprite::create("init/Home/SubMenu/bt_agreement.png");
		Sprite* agreement_btn_on  = Sprite::create("init/Home/SubMenu/bt_agreement.png");
		agreement_btn_on->setColor(Color3B(180, 180, 180));
		
		MenuItemSprite* agreement_btn = MenuItemSprite::create(agreement_btn_off, agreement_btn_on, [&](Ref* sender){
			auto normal_scene = (NormalScene *)this->getParent()->getParent();
			string url = HOST;
			url += "user/index.php?p=page__help_kiyaku&type=1";
			auto WebViewLoadLayer = WebViewLoadLayer::create(url, "利用規約");
			normal_scene->addChild(WebViewLoadLayer,9);
		});
		
		agreement_btn->setAnchorPoint(Point(0.5, 0.5));
		agreement_btn->setPosition(Point(_frame->getContentSize().width / 2, 170));
		Menu* menu = Menu::create(agreement_btn, nullptr);
		menu->setAnchorPoint(Point::ZERO);
		menu->setPosition(Point::ZERO);
		_frame->addChild(menu, 5);
		
		//はい、いいえボタンの作成
		pCanselBtn = nullptr;
		Sprite* cansel_btn_off = Sprite::create("init/Home/Common/bt_no.png");
		Sprite* cansel_btn_on = Sprite::create("init/Home/Common/bt_no.png");
		cansel_btn_on->setColor(Color3B(180, 180, 180));
		pCanselBtn = MenuItemSprite::create(cansel_btn_off, cansel_btn_on, [&](Ref* pSender)
										   {
											   auto title_scene = (TitleScene *)this->getParent();
											   title_scene->touchStart();
											   deleteAction() ;
										   });
		
		pCanselBtn->setPosition(Point(_frame->getContentSize().width / 2 + 100, 70));
		
		pOkBtn = nullptr;
		Sprite* ok_btn_off = Sprite::create("init/Home/Common/bt_yes.png");
		Sprite* ok_btn_on = Sprite::create("init/Home/Common/bt_yes.png");
		ok_btn_on->setColor(Color3B(180, 180, 180));
		pOkBtn = MenuItemSprite::create(ok_btn_off, ok_btn_on, [&](Ref* pSender){
			SaveData::saveInt("useTermsStatus", 1);
			auto title_scene = (TitleScene *)this->getParent();
			
			if(SaveData::loadInt("tutorialStatus") < 3) {
				//チュートリアルへの遷移
				title_scene->changeTutorialScene();
			} else {
				//スターティングデータ通信処理
				title_scene->getStartingDataRequest();
			}
			//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/push.mp3",false);
			closeAction();
		});
		pOkBtn->setPosition(Point(_frame->getContentSize().width / 2 - 100, 70));
		
		Menu* pMenu2 = Menu::create(pCanselBtn,pOkBtn, NULL);
		pMenu2->setAnchorPoint(Point::ZERO);
		pMenu2->setPosition(Point::ZERO);
		_frame->addChild(pMenu2);
	}
}

void ModalLayer::createFrameSetUp()
{
	//フレーム画像取得
	if (_contents == MODAL_CONTENTS_INFO || _contents == MODAL_CONTENTS_STONE) {
		_frame = Sprite::create("init/Home/Popup/madallayer.png");
	}
    else if(_contents == MODAL_CONTENTS_CHARA || _contents == MODAL_CONTENTS_POWER || _contents == MODAL_CONTENTS_EVENT_POWER || _contents == MODAL_CONTENTS_BOSS_EVENT_POWER) {
		_frame = Sprite::create("init/Home/Popup/popup_300.png");
	}
	else if(_contents == MODAL_CONTENTS_FRIEND || _contents == MODAL_CONTENTS_TERMS) {
		_frame = Sprite::create("init/Home/Popup/popup_400.png");
	}
    else if (_contents == MODAL_CONTENTS_SERIAL) {
		_frame = Sprite::create("init/Home/Popup/popup_300_s.png");
	}
    else if (_contents == MODAL_CONTENTS_GACHARATE) {
        _frame = Sprite::create("init/Home/Popup/popup_600_s.png");
    }
	
	_frame->setScale(0);
	_frame->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
	this->addChild(_frame, 2);
}

void ModalLayer::createEventListener()
{
    //イベントリスナーの作成
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    
    //シングルタップ用リスナーを用意
    //EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    //各シングルタップイベントを用意した関数に割り当てる
    listener->onTouchBegan = CC_CALLBACK_2(ModalLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ModalLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ModalLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ModalLayer::onTouchChancelled, this);
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

void ModalLayer::createShadowLayer()
{
    //影レイヤーの作成
    auto shadow_layer = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow_layer->setScale(ratio);
    shadow_layer->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow_layer, 1);
}

void ModalLayer::createCloseButton()
{
    if (_contents != MODAL_CONTENTS_GACHARATE && _contents != MODAL_CONTENTS_SERIAL && _contents != MODAL_CONTENTS_TERMS)
    {
        //クローズボタン※モーダルレイヤー削除(右上の×ボタン)
        MenuItemImage *pCloseItem = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close_L.png", [&](Ref* pSender){
            deleteAction() ;
            
        });
        pCloseItem->setAnchorPoint(Point(0.5f, 0.5f)) ;
        pCloseItem->setPosition(Point(_frame->getContentSize().width, _frame->getContentSize().height));
        Menu* pMenu = Menu::create(pCloseItem, NULL);
        pMenu->setPosition( Point::ZERO );
        _frame->addChild(pMenu);

    }
}

void ModalLayer::createButton()
{
    if (_contents != MODAL_CONTENTS_GACHARATE && _contents != MODAL_CONTENTS_SERIAL && _contents != MODAL_CONTENTS_TERMS)
    {
        //はい、いいえボタンの作成
        pCanselBtn = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/bt_no.png", [&](Ref* pSender)
                                           {
                                               deleteAction() ;
                                           });
        
        pCanselBtn->setPosition(Point(400, 100));
        
        pOkBtn = MenuItemImage::create("init/Home/Common/bt_yes.png", "init/Home/Common/bt_yes.png", [&](Ref* pSender){
            auto func = CallFunc::create([&](){
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/push.mp3",false);
//                Sprite* myParent = (Sprite*)_frame->getParent();
//                myParent->removeChild(_frame, true);
                closeAction();
            });
            auto func2 = CallFunc::create([&](){
                //DB通信リクエスト
                modalHttpRequest();
            });
            runAction(Sequence::create(func, func2, NULL));
        
        });
        pOkBtn->setPosition(Point(170, 100));
        
        Menu* pMenu2 = Menu::create(pCanselBtn,pOkBtn, NULL);
        pMenu2->setPosition(Point::ZERO);
        _frame->addChild(pMenu2);
    }
}

void ModalLayer::popupAction()
{
    //フレームを出現させる
	auto normal_scene = (NormalScene *)getParent();
	normal_scene->addTapDisableLayer() ;
	
	auto scale = ScaleTo::create(0.2f, ratio);
	_frame->runAction(scale);
	
	//指定秒数待ってからタップを可能にする。
	auto func = CallFunc::create([normal_scene](){
		normal_scene->removeTapDisableLayer() ;
	});
	
	runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

void ModalLayer::deleteAction()
{
    //フレームを削除する
	auto normal_scene = (NormalScene *)getParent();
	normal_scene->addTapDisableLayer() ;
	
	ScaleTo* scale = ScaleTo::create(0.1f, 0);
	RemoveSelf* remove = RemoveSelf::create() ;
	_frame->runAction(Sequence::create(scale,remove,nullptr));
	
	auto func = CallFunc::create([this,normal_scene](){
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
		this->removeFromParent();
		//タップ禁止レイヤー削除
		normal_scene->removeTapDisableLayer();
	});
	
	runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

void ModalLayer::closeAction()
{
    //フレームをとじるだけ（消さない）
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    ScaleTo* scale = ScaleTo::create(0.1f, 0);
    _frame->runAction(scale);
    
    auto func = CallFunc::create([this,normal_scene](){
        _frame->removeAllChildrenWithCleanup(true);
        
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer();
    });
    
    runAction(func);
}

void ModalLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの作成
    createEventListener();
    
    //影レイヤーの作成
    createShadowLayer();
    
    //フレームの作成
    createFrameSetUp();
    
    //右上の閉じるボタンの作成
    createCloseButton();
    
    //はい、いいえボタンの作成
    createButton();
    
    //ポップアップの中身の作成
    createContentsData();
    
    //ポップアップアクション
    popupAction();
}

bool ModalLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    HMSLOG("touch ModalLayer!");
	return true;
}
void ModalLayer::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	
}
void ModalLayer::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	
}
void ModalLayer::onTouchChancelled(Touch *pTouch, Event *pEvent)
{
	
}

//リクエスト
void ModalLayer::modalHttpRequest(){
	
	//auto http_connect = HttpConnect::create();
	string url = HOST;
	url += "user/user_shop_utilization.php?user_master_id=" + SaveData::loadString("user_master_id") + "&contents=" + to_string(_contents);
	auto selector = httpresponse_selector(ModalLayer::modalHttpResponse);
	HttpConnect::sendHttpRequest(this, url, selector);
}

//レスポンス
void ModalLayer::modalHttpResponse(HttpClient *sender, HttpResponse *response){
	string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
	HMSLOG("%s",result.c_str());
	
	string err;
	Json json = Json::parse(result, err);
	
	if (json.is_object())
    {
		//ユーザー情報更新
		SaveData::saveString("jewel", json["jewel"].string_value());
		SaveData::saveString("power", json["power"].string_value());
        
        //体力回復
		SaveData::saveString("max_power", json["max_power"].string_value());
        SaveData::saveString("power_use_time", json["power_use_time"].string_value());
        SaveData::saveString("timestamp_power_use_time", json["timestamp_power_use_time"].string_value());
		SaveData::saveString("max_chara_count", json["max_chara_count"].string_value());
        
        //イベント用体力の回復
		SaveData::saveString("event_power", json["event_power"].string_value());
		SaveData::saveString("max_event_power", json["max_event_power"].string_value());
        SaveData::saveString("event_power_use_time", json["event_power_use_time"].string_value());
        SaveData::saveString("timestamp_event_power_use_time", json["timestamp_event_power_use_time"].string_value());
        
        //ヘッダー内容更新
        auto header_layer = (HeaderLayer *)this->getParent()->getChildByTag(HEADER_LAYER_TAG);
    
        if (_contents == MODAL_CONTENTS_POWER && fromString<int>(json["power"].string_value()) == fromString<int>(SaveData::loadString("max_power")))
        {
            //体力更新処理
            header_layer->setNowBattlePoint(fromString<int>(json["power"].string_value()));
            header_layer->setMaxBattlePointAction();
        }

        //宝玉更新処理
        auto jewel_label = (Label *)header_layer->getChildByTag(JEWEL_LABEL_TAG);
        int new_jewel = fromString<int>(json["jewel"].string_value());
        jewel_label->setString(toString<int>(new_jewel));
        
		string report_text = json["report_text"].string_value();
		if (report_text != "")
        {
            auto return_message = HMSFontUtil::createWithTTFBySystemFontArgument(report_text, FONT_NAME_SYSTEM, 30);
            return_message->setHorizontalAlignment(TextHAlignment::CENTER);
            return_message->setPosition(Point(_frame->getContentSize() / 2));
            _frame->addChild(return_message, 1);
            
            createCloseButton();
            
            auto scale = ScaleTo::create(0.2f, ratio);
            _frame->runAction(scale);
		}
        
        //コールバックの呼び出し
        if(_parent!=nullptr && _sel!=nullptr) {
            if(_schedulerTime == 1){
                _parent->scheduleOnce(_sel, 0.0f);
                _sel = nullptr;
            }
        }
    }
    else
    {
        //閉じただけだったModalLayerを消す
        removeFromParent();
        
        //通信エラーのポップアップ表示
        auto popup = popupNoChangePageLayer::create("エラーが発生しました。");
        addChild(popup, 1);
    }
}
