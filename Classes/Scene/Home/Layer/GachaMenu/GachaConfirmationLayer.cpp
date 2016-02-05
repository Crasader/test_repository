#include "GachaConfirmationLayer.h"

GachaConfirmationLayer* GachaConfirmationLayer::create(int gacha_type)
{
    GachaConfirmationLayer *node = new GachaConfirmationLayer();
    if (node && node->init(gacha_type))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool GachaConfirmationLayer::init(int gacha_type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("GachaConfirmationLayer::init()");
    HMSLOG("init_type=%i",gacha_type);
    _gacha_type = gacha_type;//引数の受け取り
    
    return true;
}

void GachaConfirmationLayer::setData()
{
    _user_master_id = SaveData::loadString("user_master_id");
    _max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));

    if(FREE_POPUP_TAG==_gacha_type) {
        _gacha_for_count=1;
        _gacha_type=1;
        _gacha_type_label="資金";
        if(500 <= fromString<int>(SaveData::loadString("gold")))
        {
            _gacha_flg=true;
        }
    }else if(SINGLE_POPUP_TAG==_gacha_type) {
        _gacha_for_count=1;
        _gacha_type=2;
        _gacha_type_label="宝玉";
        if(1 <= fromString<int>(SaveData::loadString("jewel")))
        {
            _gacha_flg=true;
        }
    }else if(TENCONTINUOUS_POPUP_TAG==_gacha_type) {
        _gacha_for_count=10;
        _gacha_type=2;
        _gacha_type_label="宝玉";
        if(10 <= fromString<int>(SaveData::loadString("jewel")))
        {
            _gacha_flg=true;
        }
    }else if(FREE_TENCONTINUOUS_POPUP_TAG==_gacha_type) {
        _gacha_for_count=10;
        _gacha_type=1;
        _gacha_type_label="資金";
        if(5000 <= fromString<int>(SaveData::loadString("gold")))
        {
            _gacha_flg=true;
        }
    }else if(CONTINUOUS30_POPUP_TAG==_gacha_type) {
        _gacha_for_count=30;
        _gacha_type=2;
        _gacha_type_label="宝玉";
        if(30 <= fromString<int>(SaveData::loadString("jewel")))
        {
            _gacha_flg=true;
        }
    }else if(PLUS_1_POPUP_TAG==_gacha_type) {
        _gacha_for_count=1;
        _gacha_type=3;
        _gacha_type_label="宝玉";
        if(1 <= fromString<int>(SaveData::loadString("jewel")))
        {
            _gacha_flg=true;
        }
    }else if(PLUS_30_POPUP_TAG==_gacha_type) {
        _gacha_for_count=30;
        _gacha_type=3;
        _gacha_type_label="宝玉";
        int special_gacha_jewel_count = SaveData::loadInt("special_gacha_jewel_count");
        int special_gacha_chara_count = SaveData::loadInt("special_gacha_chara_count");
        _gacha_for_count = special_gacha_chara_count;
        if(special_gacha_jewel_count <= fromString<int>(SaveData::loadString("jewel")))
        {
            _gacha_flg=true;
        }
    }
}

void GachaConfirmationLayer::getSize()
{
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    main_screen_size_split_10 = main_screen_size / 10;
    main_screen_size_split_5 = main_screen_size / 5;
}

void GachaConfirmationLayer::setImg()
{
    auto scale = ScaleTo::create(0.2f, 1.0);
    this->runAction(scale);

    auto normal_scene = (NormalScene *)this->getParent();
    //影をかける(フェードイン)
    shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setTag(SHADOW_TAG) ;
    shadow->setOpacity(0) ;
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5f, 0.5f)) ;
    shadow->runAction(FadeIn::create(0.2f));
    normal_scene->addChild(shadow, ZORDER_SHADOW);
    
    //タップ禁止レイヤー生成
    AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
    this->addChild(allTapDisaleLayer,1,ALL_TAP_DISABLE_LAYER_TAG);
    
    //確認画面の枠とタイトル枠画像
    auto frame_img = Sprite::create("init/Home/Popup/popup_400_s.png");
    frame_img->setScale(ratio);
    frame_img->setPosition(convertFromDisplaySize(Point(320,450), 0.5, 0.5));
    this->addChild(frame_img,2);
    
    auto title_frame_img = Sprite::create("init/Home/Popup/popup_index_s.png");
    title_frame_img->setScale(ratio);
    title_frame_img->setPosition(convertFromDisplaySize(Point(320,590), 0.5, 0.5));
    this->addChild(title_frame_img,3);
}

void GachaConfirmationLayer::setLabel()
{
    if(_gacha_type==1) {
        title_frame_label = HMSFontUtil::createWithTTFBySystemFontArgument("無料ガチャ", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
        use_items = HMSFontUtil::createWithTTFBySystemFontArgument("必要資金: "+toString(500*_gacha_for_count), FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        have_items = HMSFontUtil::createWithTTFBySystemFontArgument("所持資金: "+SaveData::loadString("gold"), FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    }else if(_gacha_type==2) {
        title_frame_label = HMSFontUtil::createWithTTFBySystemFontArgument("ガチャ", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
        use_items = HMSFontUtil::createWithTTFBySystemFontArgument("必要宝玉: "+toString(1*_gacha_for_count)+"個", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        have_items = HMSFontUtil::createWithTTFBySystemFontArgument("所持宝玉数: "+SaveData::loadString("jewel")+"個", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    }else if(_gacha_type==3) {
        title_frame_label = HMSFontUtil::createWithTTFBySystemFontArgument("ガチャ", FONT_NAME_SYSTEM, FONT_SIZE_LIST);
        use_items = HMSFontUtil::createWithTTFBySystemFontArgument("必要宝玉: "+toString(SaveData::loadInt("special_gacha_jewel_count"))+"個", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        have_items = HMSFontUtil::createWithTTFBySystemFontArgument("所持宝玉数: "+SaveData::loadString("jewel")+"個", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    }

    title_frame_label->setScale(ratio);
    title_frame_label->setPosition(convertFromDisplaySize(Point(320,590), 0.5, 0.5));
    title_frame_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(title_frame_label, BUTTON_LABEL_Z);
    
    use_items->setScale(ratio);
    use_items->setPosition(convertFromDisplaySize(Point(320,525), 0.5, 0.5));
    use_items->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(use_items, BUTTON_LABEL_Z);
    
    have_items->setScale(ratio);
    have_items->setPosition(convertFromDisplaySize(Point(320,485), 0.5, 0.5));
    have_items->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(have_items, BUTTON_LABEL_Z);
    
    if(_gacha_flg){
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("ガチャを回します", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        label2->setScale(ratio);
        label2->setPosition(convertFromDisplaySize(Point(320,445), 0.5, 0.5));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        this->addChild(label2, BUTTON_LABEL_Z);
        
        auto label3 = HMSFontUtil::createWithTTFBySystemFontArgument("よろしいですか?", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        label3->setScale(ratio);
        label3->setPosition(convertFromDisplaySize(Point(320,405), 0.5, 0.5));
        label3->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        this->addChild(label3, BUTTON_LABEL_Z);
    }else
    {
        auto label4 = HMSFontUtil::createWithTTFBySystemFontArgument("ガチャを回す"+_gacha_type_label+"が足りません", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
        label4->setScale(ratio);
        label4->setPosition(convertFromDisplaySize(Point(320,425), 0.5, 0.5));
        label4->enableShadow(Color4B::RED, Size(2, -2), 2);
        
        //点滅させる処理
        auto fadeIn  = FadeIn::create(2.0f);
        auto fadeOut = FadeOut::create(2.0f);
        auto seq     = Sequence::create(fadeIn, fadeOut, NULL);
        auto repeat  = RepeatForever::create(seq);
        label4->runAction(repeat);

        this->addChild(label4, BUTTON_LABEL_Z);
    }
}

void GachaConfirmationLayer::setBtn()
{
    //ガチャ開始ボタン(OKボタン)
    if(_gacha_flg){
        auto Confirmation_yes_btn = MenuItemImage::create(IMG_OK_BT, IMG_OK_BT, CC_CALLBACK_1(GachaConfirmationLayer::gachaStart, this));
        Confirmation_yes_btn->setScale(ratio);
        Confirmation_yes_btn->setAnchorPoint(Point(0.5, 0.5));
        Confirmation_yes_btn->setTag(_gacha_type);//*
        auto ok_menu = Menu::create(Confirmation_yes_btn, nullptr);
        ok_menu->setPosition(convertFromDisplaySize(Point(210,330), 0.5, 0.5));
        this->addChild(ok_menu, BUTTON_POPUP_Z);//*
    } else {
        auto Confirmation_yes_btn = Sprite::create(IMG_OK_BT);
        Confirmation_yes_btn->setScale(ratio);
        Confirmation_yes_btn->setColor(Color3B(180, 180, 180));
        Confirmation_yes_btn->setPosition(convertFromDisplaySize(Point(210,330), 0.5, 0.5));
        this->addChild(Confirmation_yes_btn, BUTTON_POPUP_Z);
    }
    //ガチャキャンセルボタン(キャンセルボタン)
    auto Confirmation_no_btn = MenuItemImage::create("init/Home/Common/bt_cancel.png", "init/Home/Common/bt_cancel.png", CC_CALLBACK_1(GachaConfirmationLayer::gachaCancel, this));
    Confirmation_no_btn->setScale(ratio);
    Confirmation_no_btn->setAnchorPoint(Point(0.5, 0.5));
    Confirmation_no_btn->setTag(CANCEL_TAG);
    auto cancel_menu = Menu::create(Confirmation_no_btn, nullptr);
    cancel_menu->setPosition(convertFromDisplaySize(Point(430,330), 0.5, 0.5));
    this->addChild(cancel_menu, BUTTON_POPUP_Z);
}

void GachaConfirmationLayer::setCompletion()
{
    //ポップアップ音
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);
}

void GachaConfirmationLayer::gachaCancel(Ref* sender)
{
    shadow->runAction(Sequence::create(FadeOut::create(0.2f), RemoveSelf::create(), nullptr));
    //キャンセル音
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    auto scale = ScaleTo::create(0.2f, 0);
    this->runAction(Sequence::create(scale, RemoveSelf::create(), nullptr));
}

void GachaConfirmationLayer::gachaStart(Ref* sender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/push.mp3",false);

    string max_chara = toString(_max_chara_count);

    string url = HOST;
    url += "chara/gacha.php?";
    url += "gacha=";
    url += toString(_gacha_type);//ガチャタイプ 1~2
    url += "&userid=";
    url += toString(_user_master_id);//ユーザーマスターID(string型)
    url += "&count=";
    url += toString(_gacha_for_count);//ガチャを引く数(string)
    url += "&max_chara_count=";
    url += max_chara; //カード所持上限枚数
	
	if(_gacha_type == 3) {
		url += "&sp_gacha_tm_id=";
		url += to_string(SaveData::loadInt("special_gacha_time_management_id")); //表示中のスペシャルガチャタイムマネジメントID
	}

    auto selector = httpresponse_selector(GachaConfirmationLayer::gachaCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void GachaConfirmationLayer::gachaCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (result != "not_connect") {
        //エラーチェック
        err_check = json["err"].int_value();
        HMSLOG("err=%i", err_check);

        //エラーが発生しなかった場合
        if(err_check == 1){
            for(int i=0;i<_gacha_for_count;i++){
                //当たったカードのIDとリストID
                hit_card[i] = json[StringUtils::format("hit_card%s", toString(i).c_str())].int_value();
                chara_list_id[i] = json[StringUtils::format("chara_list_id%s", toString(i).c_str())].int_value();
                rare_card[i] = json[StringUtils::format("rare%s", toString(i).c_str())].int_value();
                chara_type[i] = json[StringUtils::format("chara_type%s", toString(i).c_str())].int_value();
                card_name_list[i] = json[StringUtils::format("name%s", toString(i).c_str())].string_value();
                HMSLOG("hit_card=%i", hit_card[i]);
            }
            _gacha_type = json["gacha_type"].int_value();
            //キャラリスト保存
            SaveData::saveString("user_chara_list", json["user_chara_list"].dump());
            //スキルデータの保存
            SaveData::saveString("user_active_skill_list", json["user_active_skill_list"].dump());
            SaveData::saveString("user_passive_skill_list", json["user_passive_skill_list"].dump());
            //宝玉・資金の情報を取得
            jewel=json["jewel"].string_value();
            gold=json["gold"].string_value();

            auto normal_scene = (NormalScene *)this->getParent();
            
            int measure_id = TimeMeasure::start() ;
            normal_scene->updateUserCharaDataByUserDefault() ;
            screenTransition();//画面遷移処理
            TimeMeasure::endAndPrint(measure_id) ;
        }
        else if (err_check == 2)//宝玉が足りていない場合
        {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
            auto yes_func = [this](){
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("ShopMenu");
                this->removeFromParent();
                normal_scene->removeChildByTag(SHADOW_TAG);
            };
            auto no_func = [](){};
            PopupStayButtonLayer *popup = PopupStayButtonLayer::create(yes_func, no_func, "宝玉が足りません。\n購入ページに移動しますか？") ;
            addChild(popup,999999) ;
        }
        else if (err_check == 3)//資金が足りていない場合
        {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
            auto popup_layer = popupNoChangePageLayer::create("資金が足りません！");
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->addChild(popup_layer, 1000);
            normal_scene->removeChildByTag(SHADOW_TAG);
            this->removeFromParent();
            HMSLOG("資金が足りない！");
        }
        else if (err_check == 4)//所持キャラクター上限を超えている場合
        {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
            auto yes_func = [this](){
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("ShopMenu");
                this->removeFromParent();
                normal_scene->removeChildByTag(SHADOW_TAG);
            };
            auto no_func = [](){};
            PopupStayButtonLayer *popup = PopupStayButtonLayer::create(yes_func, no_func, "キャラの所持上限を超えてしまいます。\n最大キャラ所持数を追加しますか？") ;
            addChild(popup,999999) ;
            HMSLOG("所持上限を超えてしまいます！");
        }
    }
}

void GachaConfirmationLayer::screenTransition(){
    //画面遷移処理
    auto normal_scene = (NormalScene *)this->getParent();

    auto load =(HeaderLayer *)normal_scene->getChildByTag(HEADER_LAYER_TAG);//ヘッダー情報取得
    
    Label* la = (Label*)load->getChildByTag(JEWEL_LABEL_TAG);//*
    Label* Gol = (Label*)load->getChildByTag(GOLD_LABEL_TAG);//*

    if(_gacha_type == 2 || _gacha_type==3)
    {
        SaveData::saveString("jewel", jewel);
        la->setString(jewel);
    }
    else if (_gacha_type == 1)
    {
        SaveData::saveString("gold", gold);
        Gol->setString(gold);
        
        //初心の手引きクリアーフラグ1 無料ガチャを引く。
        if (SaveData::loadBool("beginner_guide_clear_flg_1") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_1", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
    }
    
    
    //アニメーション呼出
    if(_gacha_type==3) {
        auto animation = PlusGachaAnimationLayer::create();
        animation->setData(_gacha_type) ;
        animation->setPosition(Point(0, 0));
        normal_scene->addChild(animation, 11);
        normal_scene->removeChildByTag(MAIN_LAYER_TAG); //メインレイヤー削除
        normal_scene->removeChildByTag(SHADOW_TAG);     //シャドーレイヤー削除
    } else {
        auto animation = GachaAnimationLayer::create();
        animation->setData(_gacha_type) ;
        animation->setPosition(Point(0, 0));
        normal_scene->addChild(animation, 11);
        normal_scene->removeChildByTag(MAIN_LAYER_TAG); //メインレイヤー削除
        normal_scene->removeChildByTag(SHADOW_TAG);     //シャドーレイヤー削除
    }
    
    //ガチャリザルトの生成
    auto title_bar_layer = normal_scene->getChildByTag(TITLE_LAYER_TAG);
    title_bar_layer->removeAllChildren();
    normal_scene->setMainName("GachaResult");
    
    auto gachaResultLayer = GachaResultLayer::create();
    gachaResultLayer->setData(_gacha_type,_gacha_for_count,hit_card,chara_list_id,rare_card,card_name_list,chara_type);
    gachaResultLayer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
    normal_scene->addChild(gachaResultLayer,ZORDER_MAIN_LAYER,MAIN_LAYER_TAG);
    this->removeFromParent();
}

void GachaConfirmationLayer::onEnter()
{
    Layer::onEnter();

    //必要な情報を取得
    setData();

    //サイズの取得
    getSize();

    //背景を生成
    setImg();
    
    //ラベルの設置
    setLabel();
    
    //ボタンの設置
    setBtn();

    //準備完了後処理
    setCompletion();
}