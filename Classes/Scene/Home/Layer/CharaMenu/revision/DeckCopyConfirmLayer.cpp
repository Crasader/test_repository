#include "DeckCopyConfirmLayer.h"

DeckCopyConfirmLayer::DeckCopyConfirmLayer()
:_origin_deck_no(1)
,_previous_deck_no(1)
,_touch_chara_index(-1)
{
}

DeckCopyConfirmLayer* DeckCopyConfirmLayer::create(int previous_deck_no)
{
    DeckCopyConfirmLayer *sprite = new DeckCopyConfirmLayer();
    
    if (sprite && sprite->init(previous_deck_no))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckCopyConfirmLayer::init(int previous_deck_no)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //選択したデッキ番号の取得
    _origin_deck_no = fromString<int>(SaveData::loadString("party_no"));
    _previous_deck_no = previous_deck_no;
    
    return true;
}

void DeckCopyConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //デッキデータの設定、取得
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //はい、いいえボタンの作成
    createButton();
    
    //マップデータの作成
    createMapData();
    
    //デッキキャラ画像の作成
    createDeckChara();
    
    //ポジションインデックスの作成
    createPositionIndex();
    
    //デッキキャラの表示
    showDeckChara();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void DeckCopyConfirmLayer::setData()
{
    //デッキ情報の取得と設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
    
}

void DeckCopyConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DeckCopyConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckCopyConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckCopyConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckCopyConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckCopyConfirmLayer::createBg()
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

void DeckCopyConfirmLayer::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 60));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("デッキコピーの確認", FONT_NAME_SYSTEM,33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(title->getContentSize() / 2));
    title->addChild(titleLabel, Z_Title, T_Title);
}

void DeckCopyConfirmLayer::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender){
        //通信処理
        copyDeckDataRequest();
    } );
    
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
        //現在のレイヤーを閉じて前のレイヤーを作成する
        auto top_layer = (DeckFormationTopLayer *)this->getParent();

        auto copy_layer = DeckCopyLayer::create();
        top_layer->addChild(copy_layer, DeckFormationTopLayer::Z_Copy_Layer, DeckFormationTopLayer::T_Copy_Layer);
        
        Utility::closePopupAction(this);
    });
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
    
    //変更確認メッセージ
    string text = "コピーを実行すると下記の内容が無くなります。";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 26, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setTextColor(Color4B(255, 120, 120, 255));
    label->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 114));
    _bg->addChild(label);
    
    //アクション
    auto fadeOut = FadeOut::create(0.8f);
    auto fadeIn  = FadeIn::create(0.4f);
    auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(3.0f), fadeOut, fadeIn, nullptr));
    label->runAction(repeat);
    
    string message = "デッキ" + toString<int>(_origin_deck_no) + " からデッキ" + toString<int>(_previous_deck_no) + "へ\nデッキ内容をコピーしてもよろしいですか？";
    auto confirm_label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(message, 28, _bg->getContentSize().width - 20));
    confirm_label->setHorizontalAlignment(TextHAlignment::CENTER);
    confirm_label->setPosition(Point(_bg->getContentSize().width / 2, 140));
    _bg->addChild(confirm_label);
}

void DeckCopyConfirmLayer::createMapData()
{
    //キャラデータのMapリストの作成
    int countJson = countJsonData(_chara_data_list);
    for (int i = 0; i < countJson; i++)
    {
        string iStr = toString<int>(i);
        _chara_list[_chara_data_list[iStr]["chara_list_id"].string_value()] = _chara_data_list[iStr].object_items();
    }
}


void DeckCopyConfirmLayer::createDeckChara()
{
    //キャラ画像の作成
    for (int i = 0; i < MAX_DECK_CHARA_COUNT; i++)
    {
        
        _chara.push_back(CharaSprite::create(_chara_list[_user_party_list[toString<int>(_previous_deck_no - 1)]["chara_list_id" + toString<int>(i + 1)].string_value()], false));
    }
    
}

void DeckCopyConfirmLayer::createPositionIndex()
{
    for (int i = 0; i < (_chara.size() / ROW_1_CHARA_COUNT) + 1; i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            _position_index.push_back(calcPositionChara(i, j));
        }
    }
}

Point DeckCopyConfirmLayer::calcPositionChara(int i, int j)
{
    int x, y;
    
    x = POS_X_DECK_FIRST + (POS_X_DECK_DISTANCE * j);
    y = POS_Y_DECK_FIRST - (POS_Y_DECK_DISTANCE * i);
    
    return Point(x, y);
}

void DeckCopyConfirmLayer::showDeckChara()
{
    //デッキキャラの表示
    
    //背景スクエア
    _chara_bg = Sprite::create("init/Home/Quest/popup_battle_deck_square2.png");
    _chara_bg->setAnchorPoint(Point(0.5, 1.0));
    _chara_bg->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 130));
    _bg->addChild(_chara_bg, Z_Chara_Bg, T_Chara_Bg);
    
    //フレンドマーク
    auto friend_sp = Sprite::create("init/CharaMenu/revision/friend.png");
    friend_sp->setPosition(Point(_position_index[0].x, _position_index[0].y + 40));
    _chara_bg->addChild(friend_sp, Z_Friend, T_Friend);
    
    
    //キャラの表示
    for (int i = 0; i < _chara.size(); i++)
    {
        _chara[i]->setScale(0.8f); //枠が小さい為、少し小さく表示する
        _chara[i]->setPosition(_position_index[i]);
        _chara_bg->addChild(_chara[i], Z_Chara, T_Chara);
    }
}

void DeckCopyConfirmLayer::copyDeckDataRequest()
{
    //デッキ変更通信処理
    string url = HOST;
    url += "chara/chara_revision/deck_copy.php";
    
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&origin_deck_no=" + toString<int>(_origin_deck_no) + "&previous_deck_no=" + toString<int>(_previous_deck_no);
    auto selector = httpresponse_selector(DeckCopyConfirmLayer::copyDeckDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void DeckCopyConfirmLayer::copyDeckDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //パーティーデータのリスト保存
        SaveData::saveString("user_party_list", json["user_party_list"].dump());
        
        //中央のスプライトを現在選択したデッキNoに更新する
        auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        deck_top_layer->setNowDeckSprite(_origin_deck_no, _previous_deck_no);
        
        //完了確認ポップアップの表示
        auto func = [](){}; //閉じるだけ
        string message = "デッキ" + toString<int>(_origin_deck_no) + " からデッキ" + toString<int>(_previous_deck_no) + "へ\nコピーが完了しました。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, true);
        deck_top_layer->addChild(popup_layer, DeckFormationTopLayer::Z_OkButton_Layer, DeckFormationTopLayer::T_OkButton_Layer);
        
        //デッキ変更成功音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo5.mp3");
        
        //閉じる
        Utility::closePopupAction(this);
    }
    else
    {
        //デッキトップの取得
        auto deck_top_layer = (DeckFormationTopLayer *)this->getParent();
        
        //デッキコピー失敗
        auto func = [deck_top_layer](){
            
            //コピー元選択画面の表示
            auto layer = DeckCopyLayer::create();
            deck_top_layer->addChild(layer, DeckFormationTopLayer::Z_Copy_Layer, DeckFormationTopLayer::T_Copy_Layer);
            
        };
        string message = "デッキのコピーに失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, true);
        this->addChild(popup_layer, DeckFormationTopLayer::Z_OkButton_Layer, DeckFormationTopLayer::T_OkButton_Layer);
        
        //閉じる
        Utility::closePopupAction(this);
    }
}

bool DeckCopyConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckCopyConfirmLayer!");
    
    return true;
}

void DeckCopyConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckCopyConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckCopyConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
