#include "DeckNamePopupLayer.h"

DeckNamePopupLayer::DeckNamePopupLayer()
:_deck_name()
{
}

DeckNamePopupLayer* DeckNamePopupLayer::create(int deck_no, string deck_name)
{
    DeckNamePopupLayer *sprite = new DeckNamePopupLayer();
    
    if (sprite && sprite->init(deck_no, deck_name))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckNamePopupLayer::init(int deck_no, string deck_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //デッキ名の初期設定
    _deck_name = deck_name;
    _deck_no   = deck_no;
    
    return true;
}

void DeckNamePopupLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //名前入力のテキストフィールドの作成
    createTextField();
    
    //はい、いいえボタンの作成
    createButton();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void DeckNamePopupLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DeckNamePopupLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckNamePopupLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckNamePopupLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckNamePopupLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckNamePopupLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_400.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //閉じるボタン
    auto close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize()));
    _bg->addChild(menu);
    
}

void DeckNamePopupLayer::createTextField()
{
    string text = "新しいデッキ名を入力してください。\n（最大8文字）";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 34, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 + 80));
    _bg->addChild(label);
    
    //テキストフィールドの作成
    auto edit_box_size = Size(330, 60);
    auto pEditbox = ui::EditBox::create(edit_box_size, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
    pEditbox->setPosition(Point(_bg->getContentSize() / 2));
    pEditbox->setInputMode(ui::EditBox::InputMode::ANY);
    pEditbox->setFont(FONT_NAME_SYSTEM, 34);
    pEditbox->setDelegate(this);
    pEditbox->setMaxLength(8);
    pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    pEditbox->setPlaceHolder(_deck_name.c_str());
    pEditbox->setPlaceholderFont(FONT_NAME_SYSTEM, 34);
    _bg->addChild(pEditbox, 2);
    
//    //背景画像の表示
//    auto text_bg = Sprite::create("init/Home/Friend/entry_column.png");
//    text_bg->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 + 10));
//    _bg->addChild(text_bg, 1);
//    
//    //テキストフィールドの作成
//    auto text_field = ui::TextField::create(_deck_name, FONT_NAME_SYSTEM, 28);
//    text_field->setPosition(Point(_bg->getContentSize() / 2));
//    text_field->setTextVerticalAlignment(TextVAlignment::CENTER);
//    text_field->setTextHorizontalAlignment(TextHAlignment::LEFT);
//    
//    text_field->setMaxLengthEnabled(true);
//    text_field->setMaxLength(8);
//    
//    text_field->addEventListener(CC_CALLBACK_2(DeckNamePopupLayer::textFieldEvent, this));
//    _bg->addChild(text_field, 2);
}

void DeckNamePopupLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void DeckNamePopupLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void DeckNamePopupLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //入力文字列の取得
    _deck_name = editBox->getText();
    HMSLOG("入力文字列 = %s", _deck_name.c_str());
}
void DeckNamePopupLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}

//void DeckNamePopupLayer::textFieldEvent(Ref* sender, cocos2d::ui::TextField::EventType type)
//{
//    auto text_field = (ui::TextField *)sender;
//    
//    switch (type) {
//        case ui::TextField::EventType::ATTACH_WITH_IME:
//        {
//            //キーボード表示時
//            CCLOG("キーボード表示時");
//            
//            text_field->setTextHorizontalAlignment(TextHAlignment::LEFT);
//        }break;
//        case ui::TextField::EventType::DETACH_WITH_IME:
//        {
//            //キーボード非表示時
//            CCLOG("キーボード非表示時");
//            
//            text_field->setTextHorizontalAlignment(TextHAlignment::CENTER);
//        }break;
//        case ui::TextField::EventType::INSERT_TEXT:
//        {
//            //文字入力時
//            CCLOG("文字入力時");
//            CCLOG("入力文字 = %s", text_field->getString().c_str());
//            
//            
//        }break;
//        case ui::TextField::EventType::DELETE_BACKWARD:
//        {
//            //文字削除時
//            CCLOG("文字削除時");
//        }break;
//        default: break;
//    }
//}

void DeckNamePopupLayer::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender){
        
        string error_message = "";
        if (calcStrLength(_deck_name) > 8) error_message = "文字数が制限を超えています。";
        if (EditBoxUtil::containInvalidChar(_deck_name) || _deck_name == "")
        {
            error_message = "無効な文字列が含まれます\n";
            error_message += EditBoxUtil::getInvalidCharsString(" ");
            error_message += "\nその他、スペース等の空白" ;
        }
        
        if (error_message != "")
        {
            //文字数がオーバーしていた、又は無効な文字列が含まれていた場合
            auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        
            auto func = [deck_top_layer](){
                
                //矢印を表示する
                deck_top_layer->_left_arrow->setVisible(true);
                deck_top_layer->_right_arrow->setVisible(true);
            };
            auto popup_layer = PopupOkButtonLayer::create(func, error_message, true);
            deck_top_layer->addChild(popup_layer, DeckFormationTopLayer::Z_Deck_Name_Complete_Layer, DeckFormationTopLayer::T_Deck_Name_Complete_Layer);
        
            //現在のレイヤーを閉じる
            Utility::closePopupAction(this);
        }
        else{
            //通信処理
            changeDeckNameDataRequest();
        }
    });
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender){
        
        auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        
        //矢印を表示する
        deck_top_layer->_left_arrow->setVisible(true);
        deck_top_layer->_right_arrow->setVisible(true);
        
        //レイヤーを閉じる
        Utility::closePopupAction(this);
    });
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
}

void DeckNamePopupLayer::changeDeckNameDataRequest()
{
    //デッキ変更通信処理
    string url = HOST;
    url += "chara/chara_revision/set_deck_name.php";
    
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&deck_no=" + toString<int>(_deck_no) + "&deck_name=" + _deck_name;
    auto selector = httpresponse_selector(DeckNamePopupLayer::changeDeckNameDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void DeckNamePopupLayer::changeDeckNameDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["check"].string_value() == "OK")
    {
        //パーティーデータのリスト保存
        SaveData::saveString("user_party_list", json["user_party_list"].dump());
        
        //デッキトップのデッキ名の変更
        auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        deck_top_layer->setNewDeckName(_deck_no);
        
        //完了確認ポップアップの表示
        auto func = [deck_top_layer](){
            
            //矢印を表示する
            deck_top_layer->_left_arrow->setVisible(true);
            deck_top_layer->_right_arrow->setVisible(true);
        };
        string message = "デッキ名を「" + json["user_party_list"][toString<int>(_deck_no - 1)]["deck_name"].string_value() + "」に\n変更しました。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, true);
        deck_top_layer->addChild(popup_layer, DeckFormationTopLayer::Z_Deck_Name_Complete_Layer, DeckFormationTopLayer::T_Deck_Name_Complete_Layer);
        
        //デッキ変更成功音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo5.mp3");
        
        //閉じる
        Utility::closePopupAction(this);
    }
	else if(json["check"].string_value() == "NG_word")
	{
		//デッキトップの取得
		auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
		
		//デッキ名変更失敗
		auto func = [deck_top_layer](){
			
			//矢印を表示する
			deck_top_layer->_left_arrow->setVisible(true);
			deck_top_layer->_right_arrow->setVisible(true);
		};
		string message = "新たなデッキ名にNGワードが\n含まれております。\nそのため、変更することができません。";
		auto popup_layer = PopupOkButtonLayer::create(func, message, true);
		deck_top_layer->addChild(popup_layer, DeckFormationTopLayer::Z_Deck_Name_Complete_Layer, DeckFormationTopLayer::T_Deck_Name_Complete_Layer);
		
		//閉じる
		Utility::closePopupAction(this);
	}
    else
    {
        //デッキトップの取得
        auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        
        //デッキ名変更失敗
        auto func = [deck_top_layer](){
        
            //矢印を表示する
            deck_top_layer->_left_arrow->setVisible(true);
            deck_top_layer->_right_arrow->setVisible(true);
        };
        string message = "デッキ名の変更に失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, true);
        deck_top_layer->addChild(popup_layer, DeckFormationTopLayer::Z_Deck_Name_Complete_Layer, DeckFormationTopLayer::T_Deck_Name_Complete_Layer);
        
        //閉じる
        Utility::closePopupAction(this);
    }
}

bool DeckNamePopupLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckNamePopupLayer!");
    return true;
}

void DeckNamePopupLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckNamePopupLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckNamePopupLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
