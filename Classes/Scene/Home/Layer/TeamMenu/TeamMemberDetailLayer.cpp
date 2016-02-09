#include "TeamMemberDetailLayer.h"

TeamMemberDetailLayer* TeamMemberDetailLayer::create(string page_name)
{
    TeamMemberDetailLayer *pRet = new TeamMemberDetailLayer();
    
    if (pRet && pRet->init(page_name))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool TeamMemberDetailLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamMemberDetailLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定
    _page_name = page_name;
    invited_comment = "";
    
    return true;
}

void TeamMemberDetailLayer::onEnter()
{
    Layer::onEnter();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamMemberDetailLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamMemberDetailLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamMemberDetailLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamMemberDetailLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //データ処理
    string team_member_detail_data = SaveData::loadString("team_member_detail_data");
    string err;
    _user_data = Json::parse(team_member_detail_data, err);
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ユーザー情報", "TeamMember", this);
    
    //ページタイトルの作成
//    createTitleBar();
    
    //ユーザ情報画像の作成
    createTeamMemberDetailSprite();
    
    //スプライトの表示
    showSprite();
    
    //ボタンの作成
    createButton();
}

void TeamMemberDetailLayer::createTitleBar()
{
}

void TeamMemberDetailLayer::createTeamMemberDetailSprite()
{
    //ユーザー情報画面の作成
    userDetail = TeamMemberDetailSprite::create();
    userDetail->setupUserData(_user_data);
    userDetail->showUserData();
    userDetail->setScale(ratio);
}

void TeamMemberDetailLayer::showSprite()
{
    //ユーザー情報の表示
    string type = _user_data["type"].string_value();
    Vec2 user_detail_pos = Point(320, 480);
    //勧誘の時は位置をずらす
    if (type == "search"){
        user_detail_pos = Point(320, 580);
    }
    
    userDetail->setPosition(convertFromDisplaySize(user_detail_pos, 0.5, 0.5));
    addChild(userDetail, 2);
}

void TeamMemberDetailLayer::createButton()
{
    string type = _user_data["type"].string_value();
    HMSLOG("type=%s", type.c_str());
    
    Sprite *img;
    Label *label;
    
    if(type=="app") {
        img = Sprite::create("init/Home/Common/bt1.png");
        img->setScale(ratio) ;
        label = HMSFontUtil::createWithTTFBySystemFontArgument("許可", FONT_NAME_SYSTEM, 30);
        label->setPosition(Point(img->getContentSize() / 2));
        img->addChild(label, 1);
        img->setPosition(convertFromDisplaySize(Point(200, 180), 0.5, 0.5));
        this->addChild(img, 1, 1);
        
        img = Sprite::create("init/Home/Common/bt1_blue.png");
        img->setScale(ratio) ;
        label = HMSFontUtil::createWithTTFBySystemFontArgument("拒否", FONT_NAME_SYSTEM, 30);
        label->setPosition(Point(img->getContentSize() / 2));
        img->addChild(label, 1);
        img->setPosition(convertFromDisplaySize(Point(440, 180), 0.5, 0.5));
        this->addChild(img, 1, 2);
    } else if(type=="soli") {
        img = Sprite::create("init/Home/Common/bt2.png");
        img->setScale(ratio) ;
        label = HMSFontUtil::createWithTTFBySystemFontArgument("勧誘取り消し", FONT_NAME_SYSTEM, 30);
        label->setPosition(Point(img->getContentSize() / 2));
        img->addChild(label, 1);
        img->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
        this->addChild(img, 1, 1);
    } else if(type=="member") {
        if(SaveData::loadBool("team_leader_flg") && _user_data["user_master_id"].string_value()!=SaveData::loadString("user_master_id")) {
            img = Sprite::create("init/Home/Common/bt1.png");
            img->setScale(ratio) ;
            label = HMSFontUtil::createWithTTFBySystemFontArgument("除名", FONT_NAME_SYSTEM, 30);
            label->setPosition(Point(img->getContentSize() / 2));
            img->addChild(label, 1);
            img->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
            this->addChild(img, 1, 1);
        }
    } else if(type=="search") {
        auto popup_bk = Sprite::create("init/Team/management/search/popup_comment.png");
        popup_bk->setScale(ratio);
        popup_bk->setPosition(convertFromDisplaySize(Point(320, 256), 0.5, 0.5));
        this->addChild(popup_bk, 0, T_BACK_POPUP);
        
        //勧誘コメント
        auto invited_comment_lebel = HMSFontUtil::createWithTTFBySystemFontArgument("勧誘コメント", FONT_NAME_SYSTEM, 24);
        invited_comment_lebel->setAnchorPoint(Point(0, 0.5));
        invited_comment_lebel->setPosition(Point(30, popup_bk->getContentSize().height - 28));
        popup_bk->addChild(invited_comment_lebel);
        
        //メッセージ入力ボックス
        Size editBoxSize = Size(popup_bk->getContentSize().width-60, 100);
        ui::EditBox* pEditbox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
        pEditbox->setPosition(Point(popup_bk->getContentSize().width /2 , popup_bk->getContentSize().height / 2 + 14));
        pEditbox->setInputMode(ui::EditBox::InputMode::ANY);
        pEditbox->setMaxLength(40);
        pEditbox->setFontColor(Color4B(0,0,0,0)); //見えない文字
        pEditbox->setFont(FONT_NAME_SYSTEM, 24);
        pEditbox->setDelegate(this);
        pEditbox->setOpacity(0);
        pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        pEditbox->setTag(T_invited_comment_box);
        if(invited_comment == ""){
            invited_comment = "チームに参加しませんか？";
            pEditbox->setText(invited_comment.c_str());
        }
        popup_bk->addChild(pEditbox, 1);
        
        
        //入力ボックス画像
        auto edit_box_sprite = Sprite::create("init/Team/management/search/bk_comment_box.png");
        edit_box_sprite->setPosition(Point(popup_bk->getContentSize().width / 2, popup_bk->getContentSize().height / 2 + 32));
        
        popup_bk->addChild(edit_box_sprite);
        
        //入力内容表示ラベル
        auto edit_box_label = HMSFontUtil::createWithTTFBySystemFontArgument(invited_comment, FONT_NAME_SYSTEM, 30);
        edit_box_label->setPosition(Point(40, 156));
        edit_box_label->setAnchorPoint(Point(0, 0.5));
        edit_box_label->setDimensions(514, 120);
        popup_bk->addChild(edit_box_label, 2, T_COMMENT_LABEL);
        
        
        img = Sprite::create("init/Team/management/search/bt_solicit.png");
        img->setScale(ratio) ;
        label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 30);
        label->setPosition(Point(img->getContentSize() / 2));
        img->addChild(label, 1);
        img->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
        this->addChild(img, 1, 1);
    } else if(type=="leader_change") {
        img = Sprite::create("init/Home/Common/bt2.png");
        img->setScale(ratio) ;
        label = HMSFontUtil::createWithTTFBySystemFontArgument("リーダー引き継ぎ", FONT_NAME_SYSTEM, 30);
        label->setPosition(Point(img->getContentSize() / 2));
        img->addChild(label, 1);
        img->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
        this->addChild(img, 1, 1);
    }
}

bool TeamMemberDetailLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamMemberDetailLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamMemberDetailLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    string type = _user_data["type"].string_value();
    
    Sprite *img;
    Rect rect;
    
    if(type=="app") {
        img = (Sprite*)this->getChildByTag(1);
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "の申請を許可してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                url += "&type=aa";
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::appYesCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
        
        img = (Sprite*)this->getChildByTag(2);
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "の申請を拒否してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                url += "&type=ar";
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::appNoCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
    } else if(type=="soli") {
        img = (Sprite*)this->getChildByTag(1);
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "への勧誘を取り消してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                url += "&type=ic";
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::cancelInvitationCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
    } else if(type=="member") {
        img = (Sprite*)this->getChildByTag(1);
        if(img==nullptr) return;
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "をメンバーから解除してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                url += "&type=re";
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::resignCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
    } else if(type=="search") {
        img = (Sprite*)this->getChildByTag(1);
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "をメンバーに勧誘してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_petition_member.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                url += "&type=i";
                url += "&invited_comment=" + URLEncode(this->invited_comment);
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::inviteCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
    } else if(type=="leader_change") {
        img = (Sprite*)this->getChildByTag(1);
        rect = img->boundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            string text = "[" + _user_data["user_name"].string_value() + "]";
            text += "\n";
            text += "にリーダーを引き継ぎます。よろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_leader_transfer.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                url += "&team_id=" + toString(SaveData::loadInt("team_id"));
                url += "&to_user_master_id=" + _user_data["user_master_id"].string_value();
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamMemberDetailLayer::changeLeaderCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
            
            return;
        }
    }
}
void TeamMemberDetailLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TeamMemberDetailLayer::appYesCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    //メッセージ表示
    if(json["result"].string_value()=="OK") {//成功
        SaveData::saveString("team_member_list_mode", "member");
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamMember");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamMemberDetailLayer::appNoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamMember");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamMemberDetailLayer::resignCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    //メッセージ表示
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamMember");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }}

void TeamMemberDetailLayer::inviteCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    //メッセージ表示
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamMember");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamMemberDetailLayer::cancelInvitationCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    //メッセージ表示
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamMember");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamMemberDetailLayer::changeLeaderCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamTop");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamMemberDetailLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    //入力開始時大きさをゼロにする
    editBox->setContentSize(Size::ZERO) ;
    HMSLOG("editBegin!");
}
void TeamMemberDetailLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    //入力開始時大きさをエディットボックスのサイズにする
    editBox->setContentSize(Size(524, 100)) ;
    HMSLOG("editEnd!");
}
void TeamMemberDetailLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    if (editBox->getTag() == T_invited_comment_box){
        //入力文字列を取得
        invited_comment = editBox->getText();
        HMSLOG("editChanged! invited_comment = %s", editBox->getText());
        
        Label *label = (Label *)this->getChildByTag(T_BACK_POPUP)->getChildByTag(T_COMMENT_LABEL) ;
        label->setString(invited_comment);
    }
}
void TeamMemberDetailLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}
