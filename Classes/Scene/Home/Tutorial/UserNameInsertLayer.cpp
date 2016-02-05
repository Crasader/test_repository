#include "UserNameInsertLayer.h"

bool UserNameInsertLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("UserNameInsertLayer::init()");
    
    ratio = getVisibleRatio();
    touchFlg = false;
    
    max_input_length = 7 ;
    
    return true;
}

void UserNameInsertLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(UserNameInsertLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(UserNameInsertLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(UserNameInsertLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void UserNameInsertLayer::createBg()
{
    //入力ポップアップ背景画像
    popup_bg = Sprite::create("init/Home/Popup/popup_200.png");
    popup_bg->setScale(ratio);
    popup_bg->setPosition(convertFromDisplaySize(Point(320, 380), 0.5, 0.5));
    addChild(popup_bg, 1);
}

void UserNameInsertLayer::createMessage()
{
    //文字の背景画像
    message_bg = MessageSprite::create("ゲームで使う名前を入力してね\n（最大７文字）");
    message_bg->setScale(ratio);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 580), 0.5, 0.5));
    addChild(message_bg, 1, MESSAGE_SPRITE_TAG);
}

void UserNameInsertLayer::createEditBox()
{
    //入力フォームの作成
    auto edit_box_size = Size(400, 70);
    auto pEditbox = ui::EditBox::create(edit_box_size, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
    pEditbox->setPosition(Point(popup_bg->getContentSize().width /2 , 136));
    pEditbox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    pEditbox->setFontColor(Color3B(255, 255, 255));
    pEditbox->setFont(FONT_NAME_SYSTEM, 36);
    pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    pEditbox->setTag(EDITBOX_TAG) ;
    popup_bg->addChild(pEditbox, 1);
}

void UserNameInsertLayer::createButton()
{
    //ボタンの作成
    auto btn_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto btn_on  = Sprite::create("init/Home/Common/bt_ok.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem = MenuItemSprite::create(btn_off, btn_on, [this](Ref* sender)
    {
        if (!touchFlg)
        {
            touchFlg = true;
            
            ui::EditBox *editBox = (ui::EditBox *)popup_bg->getChildByTag(EDITBOX_TAG) ;
            
            string input_text = editBox->getText() ;
            if (input_text == "") {
                string text = "入力欄が空白です";
                auto popup_layer = popupNoChangePageLayer::create(text);
                this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
                touchFlg = false;
            }else if(EditBoxUtil::isOverUtfLength(input_text, max_input_length)){
                string text = "最大文字数を超えています";
                auto popup_layer = popupNoChangePageLayer::create(text);
                this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
                touchFlg = false;
            }else if(EditBoxUtil::containInvalidChar(input_text)){
                string text = "無効な文字列が含まれます";
                text += "\n\n無効文字一覧  " + EditBoxUtil::getInvalidCharsString(" ");
                text += "\nその他、スペース等の空白" ;
                auto popup_layer = popupNoChangePageLayer::create(text);
                this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
                touchFlg = false;
            } else {
				string user_master_id = SaveData::loadString("user_master_id");
				string url = HOST;
				url += "user/ng_word_check.php";
				auto selector = httpresponse_selector(UserNameInsertLayer::nameCallback);
				
				string body = "user_master_id=";
				body += user_master_id;//ユーザーマスターID(string型)
				body += "&user_name=";
				body += input_text.c_str();
				HttpConnect::sendHttpRequest(this, url, selector, body);
				touchFlg = false;
			}
        }
    });
    menuItem->setAnchorPoint(Point(0.5, 0));
    auto Menu = Menu::create(menuItem, nullptr);
    Menu->setPosition(Point(popup_bg->getContentSize().width / 2 , 2));
    popup_bg->addChild(Menu, 1);
}

//変更処理のレスポンスを表示する
void UserNameInsertLayer::nameCallback(HttpClient *sender, HttpResponse *response){
	string result = HttpConnect::getHttpResponse(sender, response);
	//通信エラー以外
	if(result=="not connect"||result=="response NG") return;
	
	HMSLOG("result = %s", result.c_str());
	string err;
	Json json = Json::parse(result, err);
	
	if(json["check"].string_value() == "OK"){
		string user_name = json["user_name"].string_value();
		
		//入力したプレイヤー名の保存
		SaveData::saveString("user_name", user_name);
		
		//ポップアップにも表示させるのでメッセージ画像を見えなくさせる
		message_bg->setVisible(false);
		
		//PopupNameConfirmLayerにボタンが押されたときの処理を委譲するための無名関数を定義する
		auto yes_func = [this](){
			//ページの切り替え
			auto tutorial_scene = (TutorialScene *)this->getParent();
			auto next_layer = BattleTutorialBeforeLayer::create();
			tutorial_scene->changePage(next_layer);
		} ;
		
		auto no_func = [this](){
			//消したメッセージ画像の表示
			message_bg->setVisible(true);
		} ;
		auto popup_layer = PopupNameConfirmLayer::create(yes_func, no_func, user_name);
		this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
		
		HMSLOG("IF=%s", "OK!!!");
	}else if(json["check"].string_value()=="NG_word"){
		string text = "ニックネームにNGワードが\n含まれております";
		auto popup_layer = popupNoChangePageLayer::create(text);
		this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
	}else{
		string text = "無効な文字列が含まれます";
		auto popup_layer = popupNoChangePageLayer::create(text);
		this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
	}
}

void UserNameInsertLayer::onEnter()
{
    Layer::onEnter();
	
    //入力ポップアップ背景画像の作成
    createBg();
	
    //メッセージ画像の作成
    createMessage();
    
    //入力フォームの作成
    createEditBox();
    
    //ボタンの作成
    createButton();
}

bool UserNameInsertLayer::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("UserNameInsertLayer! touchBegan");
    return true;
}

void UserNameInsertLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void UserNameInsertLayer::onTouchEnded(Touch *touch, Event *event)
{
}