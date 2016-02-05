#include "FriendSearchLayer.h"

bool FriendSearchLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("FriendSearchLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void FriendSearchLayer::createBg()
{
    //背景の作成
    auto bg = Sprite::create("init/Home/Popup/popup_400.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(bg, 1);
}

void FriendSearchLayer::createEditBox()
{
    //自分のフレンドIDを表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("自分のID", FONT_NAME_SYSTEM, 29);
    label->setScale(ratio);
    label->setAnchorPoint(Point(0, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    label->setPosition(convertFromDisplaySize(Point(FIRST_1_X_POS, 600), 0.5, 0.5));
    addChild(label, 2);
    
    auto sp_size = Size(300 * ratio, 60 * ratio);
    auto sp = ui::Scale9Sprite::create("init/Home/Friend/entry_column.png");
    sp->setContentSize(sp_size);
    sp->setAnchorPoint(Point(0, 0.5));
    sp->setPosition(convertFromDisplaySize(Point(FIRST_1_X_POS, 540), 0.5, 0.5));
    this->addChild(sp, 2);

    string user_friend_id = SaveData::loadString("friend_id");
    auto friend_label = HMSFontUtil::createWithTTFBySystemFontArgument(user_friend_id.c_str(), FONT_NAME_SYSTEM, 40);
    friend_label->setScale(ratio);
    friend_label->setAnchorPoint(Point(0, 0.5));
    friend_label->setPosition(convertFromDisplaySize(Point(FIRST_1_X_POS + 10, 536), 0.5, 0.5));
    this->addChild(friend_label, 3);
    
    //検索対象
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("検索したいIDを入力して下さい", FONT_NAME_SYSTEM, 29);
    label2->setScale(ratio);
    label2->setAnchorPoint(Point(0, 0.5));
    label2->setPosition(convertFromDisplaySize(Point(FIRST_1_X_POS, 440), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    addChild(label2, 2);
    
    auto edit_box_size = Size(300 * ratio, 60 * ratio);
    auto pEditbox = ui::EditBox::create(edit_box_size, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
    pEditbox->setAnchorPoint(Point(0, 0.5));
    pEditbox->setPosition(convertFromDisplaySize(Point(FIRST_1_X_POS, 380), 0.5, 0.5));
    pEditbox->setInputMode(ui::EditBox::InputMode::DECIMAL);
    pEditbox->setFontColor(Color3B(255, 255, 255));
    pEditbox->setFont(FONT_NAME_SYSTEM, 40 * ratio);
    pEditbox->setDelegate(this);
    pEditbox->setMaxLength(8);
    pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    pEditbox->setPlaceHolder("友達のIDを入力");
    pEditbox->setPlaceholderFont(FONT_NAME_SYSTEM, 30 * ratio);
    this->addChild(pEditbox, 2);
}

void FriendSearchLayer::createButton()
{
     auto menuItem = MenuItemImage::create("init/Home/Friend/bt_search.png", "init/Home/Friend/bt_search.png", [&](Ref* sender)
    {
        if (input_text == "")
        {
            string text = "入力欄が空白です。";
            auto popup_layer = popupNoChangePageLayer::create(text);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        else if (input_text == SaveData::loadString("friend_id"))
        {
            string text = "自分のIDは検索できません。";
            auto popup_layer = popupNoChangePageLayer::create(text);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        else
        {
            //通信処理呼び出し
            friendSearchRequest();
        }
    });
    menuItem->setScale(ratio);
    menuItem->setAnchorPoint(Point(0, 0.5));
    auto Menu = Menu::create(menuItem, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(BUTTON_X_POS, 380), 0.5, 0.5));
    this->addChild(Menu, 1);
}

//フレンド検索リクエスト
void FriendSearchLayer::friendSearchRequest()
{
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "friend/friend_search.php";
    auto selector = httpresponse_selector(FriendSearchLayer::friendSearchCallback);
    string body = "friend_id="+input_text;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//フレンド検索レスポンス
void FriendSearchLayer::friendSearchCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["successflg"].bool_value())
    {
        //ユーザー情報ページへ
        SaveData::saveString("search_friend_id", input_text);
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("UserDetailFromFriendSearch");
    }
    else
    {
        string text = "該当するユーザはいませんでした。";
        auto popup_layer = popupNoChangePageLayer::create(text);
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void FriendSearchLayer::onEnter()
{
    Layer::onEnter();
    
    //背景の作成
    createBg();
    
    //入力フォームの作成
    createEditBox();
    
    //検索ボタンの作成
    createButton();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("フレンド検索", "FriendMenu", this);
}

void FriendSearchLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void FriendSearchLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void FriendSearchLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //入力文字列を取得
    input_text = editBox->getText();
    HMSLOG("editChanged! text = %s", editBox->getText());
}
void FriendSearchLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}
