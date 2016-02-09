#include "PopupModelChangeLayer.h"

PopupModelChangeLayer* PopupModelChangeLayer::create()
{
    PopupModelChangeLayer *node = new PopupModelChangeLayer();
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupModelChangeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupModelChangeLayer::init()");
    
    ratio = getVisibleRatio();
    
    //コールバック
    battle_list_id = 0;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupModelChangeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupModelChangeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupModelChangeLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupModelChangeLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PopupModelChangeLayer::setIdForBattleRestart(int battle_list_id)
{
    this->battle_list_id = battle_list_id;
}

void PopupModelChangeLayer::createBg()
{
    //影
    if (_shadow_visible_flg) {
        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
        shadow->setScale(ratio);
        shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(shadow, Z_SHADOW);
    }
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_400.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, 2 ,POPUP_ACTION_TAG);
}

void PopupModelChangeLayer::createMessage()
{
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("機種変更コードを入力して下さい", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, 300));
    _bg->addChild(label);
}

void PopupModelChangeLayer::createEditBox()
{
    ui::EditBox* editBox;
    Size editBoxSize = Size(400, 80);
    editBox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(IMG_TEXTBOX_BACK));
    editBox->setPosition(Point(_bg->getContentSize().width/2, 175));
    editBox->setFontColor(Color3B::WHITE);
    editBox->setMaxLength(12);
    editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    _bg->addChild(editBox, Z_EDIT_BOX, T_EDIT_BOX);
}

void PopupModelChangeLayer::createButton()
{
    //はいボタンの作成
    auto ok_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto ok_on  = Sprite::create("init/Home/Common/bt_ok.png");
    ok_on->setColor(Color3B(180, 180, 180));
    
    auto ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender)
    {
        string message = "このユーザーデータは破棄されますが、\nよろしいですか？" ;
        HMSLOG("stone message = %s", message.c_str());
        
        auto yes_func = [this](){
            modelchangeCheckRequest() ;
        };
        auto no_func = [](){};
        PopupStayButtonLayer *popup = PopupStayButtonLayer::create(yes_func, no_func, message) ;
        this->addChild(popup, Z_CONFIRM_POPUP, T_CONFIRM_POPUP);
    });
    ok_btn->setAnchorPoint(Point(0.5, 0.5));
    
    Menu* menu1 = Menu::create(ok_btn, nullptr);
    menu1->setPosition(Point(_bg->getContentSize().width / 2, 70));
    _bg->addChild(menu1);
    
    
    //閉じるボタン
    MenuItemImage* closeButton = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        //ポップアップ閉じるアクション
        deleteAction();
    });
    Menu* menu2 = Menu::create(closeButton, nullptr);
    menu2->setPosition(Point(_bg->getContentSize().width - 20, _bg->getContentSize().height - 20));
    _bg->addChild(menu2, 1);
}

//入力したIDを取得
string PopupModelChangeLayer::getTextID(){
    ui::EditBox* edtext = (ui::EditBox*)_bg->getChildByTag(T_EDIT_BOX);
    string text = edtext->getText();
    HMSLOG("ID:%s",text.c_str());
    return text;
}

//変更通信処理
void PopupModelChangeLayer::modelchangeCheckRequest(){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button2.mp3",false);
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "user/new_models.php";
    auto selector = httpresponse_selector(PopupModelChangeLayer::modelchangeCallback);
    
    string changecode = getTextID();//テキストボックスのID取得
    
    string body = "user_master_id=";
    body += SaveData::loadString("user_master_id");//ユーザーマスターID(string型)
    body += "&handling_code=";
    body += changecode.c_str();
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//変更後通信処理
void PopupModelChangeLayer::modelchangeFinishRequest(string uid){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button2.mp3",false);
    string url = HOST;
    url += "user/finish_code.php" ;
    auto selector = httpresponse_selector(PopupModelChangeLayer::modelchangeFinishCallback);
    
    string body = "user_master_id=";
    body += uid;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//変更処理のレスポンスを表示する
void PopupModelChangeLayer::modelchangeCallback(HttpClient *sender, HttpResponse *response){
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
    ui::EditBox* edbox = (ui::EditBox*)_bg->getChildByTag(T_EDIT_BOX);
    edbox->setEnabled(false);
    
    if(json["err"].string_value() == "true"){
        //登録
        SaveData::saveString("user_master_id", change_user_id);
        SaveData::saveString("password", change_password);
        
        //機種変更時はチュートリアル終了しているものとする
        SaveData::saveInt("tutorialStatus", 3);
        modelchangeFinishRequest(change_user_id);
    } else {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
        HMSLOG("%s","変更失敗！");
        
        auto popup_layer = popupChangePageLayer::create(json["message"].string_value(), "SubMenu");
        this->addChild(popup_layer, 9999999);
        this->removeChildByTag(T_CONFIRM_POPUP) ;
        
        ui::EditBox* edbox = (ui::EditBox*)_bg->getChildByTag(T_EDIT_BOX);
        edbox->setEnabled(true);
    }
}
//変更後処理
void PopupModelChangeLayer::modelchangeFinishCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    auto popuplayer = SubMenuPopup::create("SubMenu");
    popuplayer->setupImage("機種変更に成功しました。\nタイトル画面に戻ります。");
    this->addChild(popuplayer,1000);
    runAction(Sequence::create(DelayTime::create(5.0f),CallFunc::create(CC_CALLBACK_0(PopupModelChangeLayer::moveTitle,this)),NULL));
}

void PopupModelChangeLayer::moveTitle(){
    Director::getInstance()->replaceScene(TitleScene::createScene());//タイトルシーンへ
}

void PopupModelChangeLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupModelChangeLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void PopupModelChangeLayer::onEnter()
{
    Layer::onEnter();
    
    //背景画像の作成
    createBg();
    
    //文字の作成
    createMessage() ;
    
    //editboxの作成
    createEditBox() ;
    
    //ボタンの作成
    createButton();
    
    //ポップアップアクション
    popupAction();
}

bool PopupModelChangeLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupModelChangeLayer!");
    
    return true;
}
void PopupModelChangeLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupModelChangeLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupModelChangeLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}