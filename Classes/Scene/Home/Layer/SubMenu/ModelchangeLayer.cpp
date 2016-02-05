#include "ModelchangeLayer.h"

bool ModelchangeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ModelchangeLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void ModelchangeLayer::setupImage(){
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("機種変更", "Other", this);
    
    auto back_back = Sprite::create(IMG_MADALLAYER_POPUP);
    back_back->setScale(ratio);
    back_back->setPosition(convertFromDisplaySize(Point(320,450),0.5,0.5));
    this->addChild(back_back,2);
    
    ui::EditBox* editBox;
    Size editBoxSize = Size(400, 80);
    editBox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(IMG_TEXTBOX_BACK));
    editBox->setPosition(convertFromDisplaySize(Point(320, 550),0.5,0.5));
    editBox->setFontColor(Color3B::WHITE);
    editBox->setScale(ratio);
    editBox->setMaxLength(12);
    editBox->setTag(EDITBOX_TAG);
    editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    this->addChild(editBox,5);
    
    string handling_text = "機種変更コードを入力してください";
    
    auto *handling_ttf = HMSFontUtil::createWithTTFBySystemFontArgument(handling_text.c_str(), FONT_NAME_SYSTEM, 30);
    handling_ttf->setScale(ratio);
    handling_ttf->setAnchorPoint(Point(0, 1));
    handling_ttf->setPosition(convertFromDisplaySize(Point(80, 660), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	handling_ttf->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(handling_ttf, 3, 4);

    //変更開始ボタン
    MenuItemImage* go_modelchangecode = MenuItemImage::create(IMG_OK_BT, IMG_OK_BT, CC_CALLBACK_1(ModelchangeLayer::onModelChangeButton, this));
    go_modelchangecode->setScale(ratio);
    go_modelchangecode->setTag(10);
    Menu* Menu2 = Menu::create(go_modelchangecode, nullptr);
    Menu2->setPosition(convertFromDisplaySize(Point(320, 460), 0.5, 0.5));
    this->addChild(Menu2, 3, 10);
    
    //機種変更コードを発行し、コードをaddChildする
    modelchangecodeCheckRequest();

}

void ModelchangeLayer::onModelChangeButton(Ref *sender){
    
    string message = "このユーザーデータは破棄されますが、\nよろしいですか？" ;
    HMSLOG("stone message = %s", message.c_str());
    
    auto yes_func = [this](){
        modelchangeCheckRequest() ;
    };
    auto no_func = [](){};
    PopupStayButtonLayer *popup = PopupStayButtonLayer::create(yes_func, no_func, message) ;
    this->addChild(popup, 10);
}

//入力したIDを取得
string ModelchangeLayer::getTextID(){
    ui::EditBox* edtext = (ui::EditBox*)this->getChildByTag(EDITBOX_TAG);
    string text = edtext->getText();
    HMSLOG("ID:%s",text.c_str());
    return text;
}

//変更通信処理
void ModelchangeLayer::modelchangeCheckRequest(){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button2.mp3",false);
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += HTTP_MODELCHANGE_URL;
    auto selector = httpresponse_selector(ModelchangeLayer::modelchangeCallback);
    
    string changecode = getTextID();//テキストボックスのID取得
    
    string body = "user_master_id=";
    body += user_master_id;//ユーザーマスターID(string型)
    body += "&handling_code=";
    body += changecode.c_str();
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//変更後通信処理
void ModelchangeLayer::modelchangeFinishRequest(string uid){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button2.mp3",false);
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += HTTP_MODELCHANGE_FINISH_URL;
    auto selector = httpresponse_selector(ModelchangeLayer::modelchangeFinishCallback);
    
    string body = "user_master_id=";
    body += uid;//ユーザーマスターID(string型)
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//変更処理のレスポンスを表示する
void ModelchangeLayer::modelchangeCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    //変更後のuser_master_id
    string change_user_id = json["user_master_id"].string_value();
    HMSLOG("Save:user_id=%s", change_user_id.c_str());
    //変更後のpassword
    string change_password = json["password"].string_value();
    HMSLOG("Save:password=%s", change_password.c_str());
    
    //下レイヤーエディットボックス入力不可
    ui::EditBox* edbox = (ui::EditBox*)this->getChildByTag(EDITBOX_TAG);
    edbox->setEnabled(false);
    
    //下レイヤーボタンの入力不可
    Menu* button = (Menu*)this->getChildByTag(10);
    button->setEnabled(false);
    
    if(json["err"].string_value() == "true"){
        //登録
        SaveData::saveString("user_master_id", change_user_id);
        SaveData::saveString("password", change_password);
        
        modelchangeFinishRequest(change_user_id);
    } else {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
        HMSLOG("%s","変更失敗！");
        
        auto popup_layer = popupChangePageLayer::create(json["message"].string_value(), "SubMenu");
        this->addChild(popup_layer, 9999999);
        
//        auto popup_layer = popupReceiveNoneLayer::create("SubMenu");
//        popup_layer->setupImage("機種変更に失敗しました");
//        this->addChild(popup_layer, 1000);
    }
}
//変更後処理
void ModelchangeLayer::modelchangeFinishCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    auto popuplayer = SubMenuPopup::create("SubMenu");
    popuplayer->setupImage("機種変更に成功しました。\nタイトル画面に戻ります。");
    this->addChild(popuplayer,1000);
    runAction(Sequence::create(DelayTime::create(5.0f),CallFunc::create(CC_CALLBACK_0(ModelchangeLayer::moveTitle,this)),NULL));
}

//発行コード表示スプライトを閉じるボタン
void ModelchangeLayer::removeSprite(){
    Sprite* popup = (Sprite*)this->getChildByTag(MODELCHANGESPRITE_TAG);
    Label* lab = (Label*)popup->getChildByTag(MODELCHANGECODE_LABEL_TAG);
    //発行コードラベルの削除
    lab->removeFromParent();
    //下レイヤーエディットボックス入力可
    ui::EditBox* edbox = (ui::EditBox*)this->getChildByTag(EDITBOX_TAG);
    edbox->setEnabled(true);
    //下レイヤーボタンの入力可
    Menu* button = (Menu*)this->getChildByTag(10);
    button->setEnabled(true);
    //スプライトを表示外に移動
    auto move = MoveBy::create(0, convertFromDisplaySize(Point(0, 1500), 0.5, 0));
    popup->runAction(move);
}

//機種変更コード発行通信処理
void ModelchangeLayer::modelchangecodeCheckRequest(){
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += HTTP_Modelchangecode_URL;
    auto selector = httpresponse_selector(ModelchangeLayer::modelchangecodeCallback);
    
    string body = "user_master_id=";
    body += user_master_id;//ユーザーマスターID(string型)
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//機種変更コード発行通信レスポンス
void ModelchangeLayer::modelchangecodeCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    string str_code1 = "";
	string str_code2 = "";
	string str_code3 = "";
	string str_code4 = "";
	string str_code5 = "";
    if(json["err"].string_value() == "1"){
        str_code1 = "あなたの機種変更コード";
        str_code2 = json["code"].string_value();
        str_code3 = "上記コードを引継ぎ先の端末の「機種変更」で\n入力する事でデータの引継ぎが行えます。";
		str_code4 = "※機種変更コードは発行日から7日間有効です。";
    }else{
        str_code5 = json["next_time"].string_value();
    }
    auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(str_code1, FONT_NAME_SYSTEM, 24);
    label1->setScale(ratio);
    label1->setTag(MODELCHANGECODE_LABEL_TAG);
    label1->setPosition(convertFromDisplaySize(Point(320, 390), 0.5, 0.5));
	label1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(label1, 10,MODELCHANGECODE_LABEL_TAG);

	auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(str_code2, FONT_NAME_SYSTEM, 30);
	label2->setScale(ratio);
	label2->setTag(MODELCHANGECODE_LABEL_TAG);
	label2->setPosition(convertFromDisplaySize(Point(320, 360), 0.5, 0.5));
	label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	this->addChild(label2, 10,MODELCHANGECODE_LABEL_TAG);

	auto label3 = HMSFontUtil::createWithTTFBySystemFontArgument(str_code3, FONT_NAME_SYSTEM, 20);
	label3->setScale(ratio);
	label3->setTag(MODELCHANGECODE_LABEL_TAG);
	label3->setPosition(convertFromDisplaySize(Point(320, 300), 0.5, 0.5));
	label3->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	this->addChild(label3, 10,MODELCHANGECODE_LABEL_TAG);
	
	auto label4 = HMSFontUtil::createWithTTFBySystemFontArgument(str_code4, FONT_NAME_SYSTEM, 20);
	label4->setScale(ratio);
	label4->setTag(MODELCHANGECODE_LABEL_TAG);
	label4->setPosition(convertFromDisplaySize(Point(320, 260), 0.5, 0.5));
	label4->setColor(Color3B::RED);
	label4->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	this->addChild(label4, 10,MODELCHANGECODE_LABEL_TAG);

	//エラーメッセージ用
	auto label5 = HMSFontUtil::createWithTTFBySystemFontArgument(str_code5, FONT_NAME_SYSTEM, 30);
	label5->setScale(ratio);
	label5->setTag(MODELCHANGECODE_LABEL_TAG);
	label5->setPosition(convertFromDisplaySize(Point(320, 340), 0.5, 0.5));
	label5->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	this->addChild(label5, 10,MODELCHANGECODE_LABEL_TAG);

}

void ModelchangeLayer::moveTitle(){
    Director::getInstance()->replaceScene(TitleScene::createScene());//タイトルシーンへ
}

void ModelchangeLayer::changePage(Ref* sender){
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("SubMenu");
    HMSLOG("touch back!");
}

void ModelchangeLayer::onEnter(){
    Layer::onEnter();

    setupImage();
}