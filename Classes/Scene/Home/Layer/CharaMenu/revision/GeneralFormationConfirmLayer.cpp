#include "GeneralFormationConfirmLayer.h"

GeneralFormationConfirmLayer::GeneralFormationConfirmLayer()
{
}

GeneralFormationConfirmLayer* GeneralFormationConfirmLayer::create(CharaSprite* chara)
{
    GeneralFormationConfirmLayer *sprite = new GeneralFormationConfirmLayer();
    
    if (sprite && sprite->init(chara))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool GeneralFormationConfirmLayer::init(CharaSprite* chara)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //セルリストの初期化
    _chara_data = chara;
    
    return true;
}

void GeneralFormationConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //はい、いいえボタンの作成
    createButton();
    
    //変更総大将キャラ画像の作成
    createGeneralChara();
    
    //総大将キャラの表示
    showGeneralChara();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void GeneralFormationConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GeneralFormationConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GeneralFormationConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GeneralFormationConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(GeneralFormationConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GeneralFormationConfirmLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_700.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
}

void GeneralFormationConfirmLayer::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("総大将確認", FONT_NAME_SYSTEM,33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(titleLabel, Z_Title, T_Title);
}

void GeneralFormationConfirmLayer::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender){
        //通信処理
        generalCharaDataRequest();
    } );
    
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender){
        //閉じる処理
        Utility::closePopupAction(this);
    });
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
    
    //確認メッセージ
    auto sell_confirm_label = HMSFontUtil::createWithTTFBySystemFontArgument("このキャラを総大将に設定しますか？", FONT_NAME_SYSTEM, 28);
    sell_confirm_label->setPosition(Point(_bg->getContentSize().width / 2, 120));
    _bg->addChild(sell_confirm_label);
}

void GeneralFormationConfirmLayer::createGeneralChara()
{
    //キャラ画像の作成
    _chara.push_back(CharaSprite::create(createCharaJsonData(), false, CharaSprite::CardSize::ALL_400));
}

Json GeneralFormationConfirmLayer::createCharaJsonData()
{
    //キャラのJsonデータを作成 (C+11からの初期化子リストでコンテナの初期化を使用している)
    Json json = Json::object {
        {"chara_list_id", _chara_data->getCharaListId()},
        {"chara_id", _chara_data->getCharaId()},
        {"chara_no", _chara_data->getCharaNo()},
        {"second_name", _chara_data->getSecondName()},
        {"name", _chara_data->getName()},
        {"name_kana", _chara_data->getNameKana()},
        {"rare", _chara_data->getRare()},
        {"chara_type", toString<int>((int)_chara_data->getCharaType())},
        {"left_joint", toString<int>((int)_chara_data->getLeftJoint())},
        {"right_joint", toString<int>((int)_chara_data->getRightJoint())},
        {"hp", _chara_data->getHp()},
        {"power", _chara_data->getPower()},
        {"need_cost", _chara_data->getNeedCost()},
        {"add_cost", _chara_data->getAddCost()},
        {"turn_count", _chara_data->getTurnCount()},
        {"active_skill_id", _chara_data->getActiveSkillId()},
        {"passive_skill_id", _chara_data->getPassiveSkillId()},
        {"chara_comment", _chara_data->getCharaComment()},
        {"add_timing", _chara_data->getAddTiming()},
        {"insert_date", _chara_data->getInsertDate()},
    };
    
    return json;
}


void GeneralFormationConfirmLayer::showGeneralChara()
{
    //総大将の表示
    _chara[0]->setPosition(Point(_bg->getContentSize() / 2));
    _bg->addChild(_chara[0]);
    
}

void GeneralFormationConfirmLayer::generalCharaDataRequest()
{
    //総大将変更通信処理
    string url = HOST;
    url += "chara/chara_revision/set_general.php";
    
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&general_list_id=" + _chara[0]->getCharaListId() + "&deck_no=" + SaveData::loadString("party_no");
    auto selector = httpresponse_selector(GeneralFormationConfirmLayer::generalCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void GeneralFormationConfirmLayer::generalCharaDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //パーティーデータリスト保存
        SaveData::saveString("user_party_list", json["user_party_list"].dump());
        
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        
        //確認ポップアップの表示
        Utility::hidePopupAction(this);
        auto func = [this, normal_scene](){
            
            normal_scene->changePage("DeckFormationTop");
            
            Utility::closePopupAction(this);
        };
        string message =  "総大将を" + _chara[0]->getName() + "に変更しました。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, 999999);
    }
    else
    {
        //総大将確認
        Utility::hidePopupAction(this);
        auto func = [this](){
            Utility::openPopupAction(this);
        };
        string message = "総大将更新に失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, 999999);
    }
}

bool GeneralFormationConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan GeneralFormationConfirmLayer!");
    
    return true;
}
void GeneralFormationConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void GeneralFormationConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void GeneralFormationConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}