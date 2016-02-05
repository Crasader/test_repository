#include "NicknameLayer.h"

bool NicknameLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("NicknameLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void NicknameLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("名前変更", "Profile", this);
    
    //画像の設定
    setupImage();
}

void NicknameLayer::setupImage()
{
    auto bg = Sprite::create("init/Home/Popup/popup_500.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 4, 1);
    
    string span = SaveData::loadString("user_name_change_span");
    
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument("新しい名前を入力してください\n(最大7文字)\n(変更後" + span + "は再変更できません)", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    name_label->setHorizontalAlignment(TextHAlignment::CENTER);
    name_label->setPosition(Point(bg->getContentSize().width / 2, 400));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    bg->addChild(name_label,5);
    
    ui::EditBox* editBox;
    Size editBoxSize = Size(400, 80);
    editBox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(IMG_TEXTBOX_BACK));
    editBox->setPosition(Point(bg->getContentSize().width / 2, 250));
    editBox->setFontColor(Color3B::WHITE);
    editBox->setFont(FONT_NAME_SYSTEM, 36);
    editBox->setTag(EDITBOX_TAG);
    editBox->setText(SaveData::loadString("user_name").c_str());
    editBox->setInputMode(ui::EditBox::InputMode::ANY);
    editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    bg->addChild(editBox,5);
    
    //ボタン
    auto Commentbt = MenuItemImage::create(IMG_OK_BT, IMG_OK_BT, CC_CALLBACK_1(NicknameLayer::nicknameRequest, this));
    Commentbt->setTag(10);
    Commentbt->setAnchorPoint(Point(0.5, 0));
    auto Menu2 = Menu::create(Commentbt, nullptr);
    Menu2->setPosition(Point(bg->getContentSize().width / 2, 20));
    bg->addChild(Menu2, 5, 10);
}

//変更通信処理
void NicknameLayer::nicknameRequest(Ref *sender){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button2.mp3",false);
    
    ui::EditBox* editBox =(ui::EditBox*)this->getChildByTag(1)->getChildByTag(EDITBOX_TAG);
    string input_text = editBox->getText() ;
    if (input_text == "") {
        string text = "入力欄が空白です";
        auto popup_layer = popupNoChangePageLayer::create(text);
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }else if(EditBoxUtil::isOverUtfLength(input_text, 7)){
        string text = "最大文字数を超えています";
        auto popup_layer = popupNoChangePageLayer::create(text);
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }else if(EditBoxUtil::containInvalidChar(input_text)){
        string text = "無効な文字列が含まれます";
        text += "\n\n無効文字一覧  " + EditBoxUtil::getInvalidCharsString(" ");
        text += "\nその他、スペース等の空白" ;
        auto popup_layer = popupNoChangePageLayer::create(text);
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    } else {
        //auto http_connect = HttpConnect::create();
        string url = HOST;
        url += NICK_NAME_URL;
        auto selector = httpresponse_selector(NicknameLayer::nameCallback);
        
        string body = "user_master_id=";
        body += user_master_id;//ユーザーマスターID(string型)
        body += "&user_name=";
        body += input_text.c_str();
        HttpConnect::sendHttpRequest(this, url, selector, body);
    }
}

//変更処理のレスポンスを表示する
void NicknameLayer::nameCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["bool"].string_value() == "OK"){

        //ヘッダーラベルの変更
        auto name_label = (Label *)this->getParent()->getChildByTag(HEADER_LAYER_TAG)->getChildByTag(NAME_LABEL_TAG);
        name_label->setString(json["user_name"].string_value());
        SaveData::saveString("user_name", json["user_name"].string_value());
        
        auto popup_layer = ProfilePopup::create("ProfilePopup","ProfilePopup");
        popup_layer->setupImage("ニックネームの変更が完了しました！","ProfilePopup");
        this->addChild(popup_layer, 1000);
        
        HMSLOG("IF=%s", "OK!!!");
    }else if(json["bool"].string_value()=="NG_date"){
        string date = json["date"].string_value();
        string span = json["span"].string_value();
        auto popup_layer = ProfilePopup::create("ProfilePopup","ProfilePopup");
        popup_layer->setupImage("ニックネームは一度変更を行うと、\n" + span + "は変更できません。\n\n前回の変更日時\n[" + date + "]", "ProfilePopup");
        this->addChild(popup_layer, 1000);
	}else if(json["bool"].string_value()=="NG_word"){
		auto popup_layer = ProfilePopup::create("ProfilePopup","ProfilePopup");
		popup_layer->setupImage("新たなニックネームに\nNGワードが含まれております。\nそのため、変更することができません。", "ProfilePopup");
		this->addChild(popup_layer, 1000);
	}else{
        auto popup_layer = ProfilePopup::create("ProfilePopup","ProfilePopup");
        popup_layer->setupImage("ニックネームの変更に失敗しました...","ProfilePopup");
        this->addChild(popup_layer, 1000);
    }
}

void NicknameLayer::changePage(Ref* sender){
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("Profile");
    HMSLOG("touch back!");
}