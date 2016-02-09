#include "LoginBounsPopupLayer.h"

LoginBounsPopupLayer::LoginBounsPopupLayer()
:_login_campaign_title()
,_item_type()
,_chara_name()
,_related_primary_id()
,_present_count(0)
,_loop_count(0)
{
}

LoginBounsPopupLayer* LoginBounsPopupLayer::create(Json json)
{
    LoginBounsPopupLayer *node = new LoginBounsPopupLayer();
    if (node && node->init(json))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool LoginBounsPopupLayer::init(Json json)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("LoginBounsPopupLayer::init()");
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _login_present_list = json;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(LoginBounsPopupLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(LoginBounsPopupLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(LoginBounsPopupLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(LoginBounsPopupLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    return true;
}

void LoginBounsPopupLayer::parsePresentList()
{
    //プレゼントリストの分解
    int json_count = countJsonData(_login_present_list) ;
    for (int i = 0; i < json_count; i++) {
        string num = toString<int>(i);
        _present_data.push_back(_login_present_list[num]);
    }
}

void LoginBounsPopupLayer::setPresentData()
{
    //プレゼントデータの設定
    setLoginCampaignTitle(_present_data[0]["login_campaign_title"].string_value());
    setItemType(_present_data[0]["item_type"].string_value());
    setCharaName(_present_data[0]["chara_name"].string_value());
    setRelated_primary_id(_present_data[0]["related_primary_id"].string_value());
    setPresentCount(fromString<int>(_present_data[0]["count"].string_value()));
    
    HMSLOG("campaigntitle = %s, itemtype = %s, presentcount = %d, charaname= %s, primary_id = %s", _login_campaign_title.c_str(), _item_type.c_str(), _present_count, _chara_name.c_str(), _related_primary_id.c_str());
}

void LoginBounsPopupLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_600.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, Utility::Z_Popup_Action ,Utility::T_Popup_Action);
}

void LoginBounsPopupLayer::createButton()
{
    //OKボタンの作成
    auto ok_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto ok_on  = Sprite::create("init/Home/Common/bt_ok.png");
    ok_on->setColor(Color3B(180, 180, 180));
    
    _ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender)
    {
        //ポップアップ閉じる
        Utility::closePopupAction(this);
        
        auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
        auto normal_scene = (NormalScene *)mypage_layer->getParent() ;
        if(mypage_layer && normal_scene->getMainName()=="")mypage_layer->showNextPopupStatus();
        
//        deleteAction();
//        
//        //ビギナーリスト項目達成チェック（達成していた場合自動敵に開く）
//        auto mypage_layer = (MypageLayer *)this->getParent();
//        
//        if (SaveData::loadString("beginner_guide_complete_flg") == "N")
//        {
//            //ビギナーフラグ達成かをチェックし、自動的に表示させる処理
//            mypage_layer->checkOpenCheckBeginnerGuideLayer();
//        }
    });
    _ok_btn->setAnchorPoint(Point(0.5, 0));
    
    auto menu = Menu::create(_ok_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 40));
    _bg->addChild(menu, 1);
}

void LoginBounsPopupLayer::showCampaignTitle()
{
    int title_font_size = 50;
    string title = _login_campaign_title;
    
    if (calcStrLength(title) > 10 ) {
        //10文字以上の場合一文字につき、3ポイントサイズを小さくする（17文字ぐらいが限界かも）
        title_font_size = 50 - (calcStrLength(title) - 10) * 3;
        if (title_font_size <= 30) {
            title_font_size = 30;
        }
    }
    
    //キャンペーン名の表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(title, FONT_NAME_SYSTEM, title_font_size);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, 530));
    _bg->addChild(label, 2);
}

void LoginBounsPopupLayer::createPresentSprite()
{
    //プレゼント画像の作成(デフォルトの画像を設定)
    _present_sp = Sprite::create("download/Chara/parts400/card_bk1_reverse.png");
    _present_sp->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height /2 + 30));
    _bg->addChild(_present_sp, 2);
}

void LoginBounsPopupLayer::createPresentName()
{
    //プレゼント名の作成
    _present_name_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    _present_name_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _present_name_label->setAnchorPoint(Point(0.5, 0));
    _present_name_label->setPosition(Point(_bg->getContentSize().width / 2, _ok_btn->getPosition().y + _ok_btn->getContentSize().height + 34));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    _bg->addChild(_present_name_label, 2);
}

void LoginBounsPopupLayer::changePresentData()
{
    //画像の切り替え
    if (_item_type == "chara") {
        string card_pass = "download/Chara/all400/" + _related_primary_id + ".png";
        _present_sp->setTexture(card_pass);
        _present_name_label->setString(_chara_name + " × " + toString<int>(_present_count) + " を受け取りました");
    }
    else if (_item_type == "gold") {
        _present_sp->setTexture("init/Present/item2_400.png");
        _present_name_label->setString("資金 × " + toString<int>(_present_count) + " を受け取りました");
    }
    else if (_item_type == "jewel") {
        _present_sp->setTexture("init/Present/item_400.png");
        _present_name_label->setString("宝玉 × " + toString<int>(_present_count) + " を受け取りました");
    }
    
    _present_sp->setScale(0.7);
}

void LoginBounsPopupLayer::showMessage()
{
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("※受け取ったアイテムはプレゼントボックスに送られます", FONT_NAME_SYSTEM, 18);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setAnchorPoint(Point(0.5, 0));
    label->setPosition(Point(_bg->getContentSize().width / 2, 25));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    _bg->addChild(label, 2);
}

//void LoginBounsPopupLayer::popupAction()
//{
//    auto children = this->getChildren();
//    
//    for (auto child : children)
//    {
//        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
//        child->runAction(scale);
//    }
//}
//
//void LoginBounsPopupLayer::deleteAction()
//{
//    auto children = this->getChildren();
//    
//    for (auto child : children)
//    {
//        if (child->getTag() == POPUP_ACTION_TAG)
//        {
//            auto scale    = ScaleTo::create(0.1f, 0);
//            child->runAction(scale);
//        }
//    }
//    
//    auto func = CallFunc::create([this](){
//        this->removeFromParent();
//    });
//    
//    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
//}

bool LoginBounsPopupLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch LoginBounsPopupLayer!");
    
    return true;
}
void LoginBounsPopupLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void LoginBounsPopupLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void LoginBounsPopupLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void LoginBounsPopupLayer::onEnter()
{
    Layer::onEnter();
    
    //プレゼントデータの分解
    parsePresentList();
    
    //プレゼントデータの設定
    setPresentData();
    
    //背景画像の作成
    createBg();
    
    //ボタンの作成
    createButton();
    
    //キャンペーンタイトルの表示
    showCampaignTitle();
    
    //プレゼント画像の作成
    createPresentSprite();
    
    //プレゼント名の作成
    createPresentName();
    
    //画像、名前の切り替え
    changePresentData();
    
    //メッセージの表示
    showMessage();
    
    //ポップアップの表示
    Utility::openPopupAction(this);
//    popupAction();
}