#include "CharaEvolutionConfirmLayer.h"
#include "EvolutionAnimationLayer.h"
#include "EvolutionResultLayer.h"

bool CharaEvolutionConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaEvolutionConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void CharaEvolutionConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("キャラ進化確認", "CharaEvolution", this);
    
    //イベントリスナーの設定
    initEventListener();
    
    //
    getCharaDataRequest();
}

void CharaEvolutionConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaEvolutionConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaEvolutionConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaEvolutionConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaEvolutionConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaEvolutionConfirmLayer::getCharaDataRequest()
{
    //キャラIDの設定
    _chara_id = SaveData::loadString("evolution_chara_id");
    
    //HTTPリクエスト
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "chara/chara_revision/get_evolution_chara_info.php";
    auto selector = httpresponse_selector(CharaEvolutionConfirmLayer::charaDataCallback);
    url += "?user_master_id=" + user_master_id;
    url += "&chara_id=" + _chara_id;
    HttpConnect::sendHttpRequest(this, url, selector);
}

void CharaEvolutionConfirmLayer::charaDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result=%s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    //_next_chara = CharaSprite::create(json["chara_data"].object_items(), false, CharaSprite::CardSize::ALL_400);
    _next_chara = CharaSprite::create(json["chara_data"].object_items());
    _next_chara_id = _next_chara->getCharaId();
    //_next_chara_list_id = _next_chara->getCharaListId();
    _next_chara_active_skill_name = json["active_skill_name"].string_value();
    _next_chara_active_skill_cost = json["active_skill_cost"].string_value();
    _next_chara_passive_skill_name = json["passive_skill_name"].string_value();
    _pre_chara_active_skill_name = json["active_skill_name0"].string_value();
    _pre_chara_active_skill_cost = json["active_skill_cost0"].string_value();
    _pre_chara_passive_skill_name = json["passive_skill_name0"].string_value();
    
    setup();
}

void CharaEvolutionConfirmLayer::setup()
{
    //キャラデータの設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);

    //キャラ画像の作成
    int count = 0;
    int countJson = countJsonData(_chara_data_list);
    for(int i=0; i<countJson; i++) {
        string iStr = toString(i);
        Json chara_data = _chara_data_list[iStr].object_items();
        string chara_id = chara_data["chara_id"].string_value();
        if(chara_id==_chara_id) {
            _chara.push_back(CharaSprite::create(_chara_data_list[iStr].object_items()));
            count++;
        }
        if(count>=3) break;
    }
    
    if(count<3) return;//3枚以下はありえない
    
    //背景
    Sprite *img;
    img = Sprite::create("init/CharaMenu/evolution/bk_evolution1.png");
    img->setPosition(convertFromDisplaySize(Point(320, 664), 0.5, 0.5));
    img->setScale(ratio);
    this->addChild(img);
    img = Sprite::create("init/CharaMenu/evolution/bk_evolution2.png");
    img->setPosition(convertFromDisplaySize(Point(320, 284), 0.5, 0.5));
    img->setScale(ratio);
    this->addChild(img);
    img = Sprite::create("init/CharaMenu/evolution/bk_evolution3.png");
    img->setPosition(convertFromDisplaySize(Point(373, 485), 0.5, 0.5));
    img->setScale(ratio);
    this->addChild(img);
    img = Sprite::create("init/CharaMenu/evolution/arrow.png");
    img->setPosition(convertFromDisplaySize(Point(142, 480), 0.5, 0.5));
    img->setScale(ratio);
    this->addChild(img);
    
    //進化前キャラ表示
    _chara[0]->setPosition(convertFromDisplaySize(Point(114, 654), 0.5, 0.5));
    _chara[0]->setScale(ratio);
    this->addChild(_chara[0], 1, 2);
    _chara[1]->setPosition(convertFromDisplaySize(Point(315, 474), 0.5, 0.5));
    _chara[1]->setScale(ratio);
    this->addChild(_chara[1], 1, 2);
    _chara[2]->setPosition(convertFromDisplaySize(Point(442, 474), 0.5, 0.5));
    _chara[2]->setScale(ratio);
    this->addChild(_chara[2], 1, 2);

    
    //進化後キャラ
    _next_chara->setPosition(convertFromDisplaySize(Point(114, 274), 0.5, 0.5));
    _next_chara->setScale(ratio);
    this->addChild(_next_chara, 1, 3);

    //テキスト
    string text;
    Label *label;

    text = "※進化前キャラと同じキャラ2体を使用します";
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 23*ratio);
    //label->enableOutline(Color4B::BLACK, 1);
    //label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(150, 387), 0.5, 0.5));
    this->addChild(label, 1);
    
    text = "発動士気";
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 22*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(440, 290), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_next_chara_active_skill_cost);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 28*ratio);
    label->setAnchorPoint(Point(1, 0.5));
    label->setPosition(convertFromDisplaySize(Point(557, 290), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_next_chara_active_skill_name);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(190, 290), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_next_chara_passive_skill_name);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(190, 233), 0.5, 0.5));
    label->setColor(Color3B(255,221,0));
    this->addChild(label, 1);
    
    text = "発動士気";
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 22*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(440, 670), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_pre_chara_active_skill_cost);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 28*ratio);
    label->setAnchorPoint(Point(1, 0.5));
    label->setPosition(convertFromDisplaySize(Point(557, 670), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_pre_chara_active_skill_name);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(190, 670), 0.5, 0.5));
    label->setColor(Color3B(255,153,0));
    this->addChild(label, 1);
    
    text = toString(_pre_chara_passive_skill_name);
    label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24*ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(190, 613), 0.5, 0.5));
    label->setColor(Color3B(255,221,0));
    this->addChild(label, 1);
    
    //必要資金
    img = Sprite::create("download/CharaMenu/evolution_price.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(185, 150), 0.5, 0.5));
    this->addChild(img, 1);
    
    //ボタン
    string filename = "init/CharaMenu/evolution/bt_evolution2.png";
    int gold = fromString<int>(SaveData::loadString("gold"));
    HMSLOG("gold=%d,price=%d", gold, SaveData::loadInt("evolution_price"));
    if(gold >= SaveData::loadInt("evolution_price")) {
        enable_evolution_flg = true;
        filename = "init/CharaMenu/evolution/bt_evolution.png";
    }
    img = Sprite::create(filename.c_str());
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(460, 150), 0.5, 0.5));
    this->addChild(img, 1, 1);
    
    //矢印アニメーション1
    Point base_point = Point(142, 530);
    Point start_point = convertFromDisplaySize(base_point, 0.5, 0.5);
    Point end_point = convertFromDisplaySize(Point(base_point.x, base_point.y - 90), 0.5, 0.5);
    
    ClippingNode *node = ClippingNode::create();
    node->setAlphaThreshold(0.0f);
    this->addChild(node, 1);

    img = Sprite::create("init/CharaMenu/evolution/arrow_mask1.png");
    img->setPosition(convertFromDisplaySize(Point(142, 480), 0.5, 0.5));
    img->setScale(ratio);
    node->setStencil(img);
    
    img = Sprite::create("init/CharaMenu/evolution/arrow_gradation.png");
    img->setPosition(start_point);
    img->setScale(ratio);
    //this->addChild(img);
    node->addChild(img);
    
    Sequence *seq = Sequence::create(DelayTime::create(0.0f), MoveTo::create(2.0f, end_point), Place::create(start_point),  nullptr);
    RepeatForever *repeat = RepeatForever::create(seq);
    img->runAction(repeat);
    
    //矢印アニメーション2
    base_point = Point(235, 474);
    start_point = convertFromDisplaySize(base_point, 0.5, 0.5);
    end_point = convertFromDisplaySize(Point(base_point.x-90, base_point.y), 0.5, 0.5);
    
    ClippingNode *node2 = ClippingNode::create();
    node2->setAlphaThreshold(0.0f);
    this->addChild(node2, 1);
    
    img = Sprite::create("init/CharaMenu/evolution/arrow_mask2.png");
    img->setPosition(convertFromDisplaySize(Point(142, 480), 0.5, 0.5));
    img->setScale(ratio);
    node2->setStencil(img);
    
    img = Sprite::create("init/CharaMenu/evolution/arrow_gradation.png");
    img->setPosition(start_point);
    img->setRotation(90);
    img->setScale(ratio);
    node2->addChild(img);
    
    Sequence *seq2 = Sequence::create(DelayTime::create(0.0f), MoveTo::create(2.0f, end_point), Place::create(start_point),  nullptr);
    RepeatForever *repeat2 = RepeatForever::create(seq2);
    img->runAction(repeat2);
}

bool CharaEvolutionConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaEvolutionConfirmLayer!");
    
    return true;
}
void CharaEvolutionConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaEvolutionConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Sprite *img;
    img = (Sprite *)this->getChildByTag(1);
    if(Utility::containsPoint(img, touch->getLocation())) {
        HMSLOG("yes");
        //doEvolutionRequest();
        //資金の有無をチェック
        if(enable_evolution_flg==true) {
            //ポップアップ表示
            auto ok_func = [this](){
                this->doEvolutionRequest();
            };
            
            auto ng_func = [](){}; //いいえを押したとき何もしない
            popupConfirmLayer *layer = popupConfirmLayer::create("3体のURを合成することで\n新たな能力を持った進化キャラ（UR+）を\n1体手に入れることができます。\n進化させますか？");
            layer->setFunction(ok_func, ng_func);
            this->addChild(layer, 100);
        }
        return;
    }
    
    /*
    img = (Sprite *)this->getChildByTag(2);
    if(Utility::containsPoint(img, touch->getLocation())) {
        HMSLOG("no");
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("CharaEvolution");
    }
     */
    
    img = (Sprite *)this->getChildByTag(2);
    if(Utility::containsPoint(img, touch->getLocation())) {
        HMSLOG("detail");
        //キャラ詳細の表示
        //HMSLOG("chara_id=%s, chara_list_id=%s", _next_chara->getCharaId().c_str(), _next_chara->getCharaListId().c_str()ƒ);
        auto detail_layer = CardDetailLayer::create(_chara_id, "");
        this->addChild(detail_layer, 10);
        
        //閉じられたときによばれる関数の指定
        auto close_func = [this](){
            //checkFavoriteChara();
        };
        detail_layer->setCloseFunc(close_func);
        
        return;
    }
    
    img = (Sprite *)this->getChildByTag(3);
    if(Utility::containsPoint(img, touch->getLocation())) {
        HMSLOG("detail");
        //キャラ詳細の表示
        //HMSLOG("chara_id=%s, chara_list_id=%s", _next_chara->getCharaId().c_str(), _next_chara->getCharaListId().c_str()ƒ);
        auto detail_layer = CardDetailLayer::create(_next_chara_id, "");
        this->addChild(detail_layer, 10);
        
        //閉じられたときによばれる関数の指定
        auto close_func = [this](){
            //checkFavoriteChara();
        };
        detail_layer->setCloseFunc(close_func);
        
        return;
    }
}

void CharaEvolutionConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void CharaEvolutionConfirmLayer::doEvolutionRequest()
{
    //キャラIDの設定
    _chara_id = SaveData::loadString("evolution_chara_id");
    
    //HTTPリクエスト
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "chara/chara_revision/do_evolution.php";
    auto selector = httpresponse_selector(CharaEvolutionConfirmLayer::doEvolutionCallback);
    url += "?user_master_id=" + user_master_id;
    url += "&chara_id=" + _chara_id;
    HttpConnect::sendHttpRequest(this, url, selector);
}

void CharaEvolutionConfirmLayer::doEvolutionCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result=%s", result.c_str());

    string err;
    Json json = Json::parse(result, err);
    
    if(result != "not_connect") {
        //エラーチェック
        int err_check = json["err"].int_value();
        HMSLOG("err=%i", err_check);
        
        //エラーが発生しなかった場合
        if(err_check== 1) {
            for(int i=0;i<1;i++){
                //当たったカードのIDとリストID
                hit_card[i] = json[StringUtils::format("hit_card%s", toString(i).c_str())].int_value();
                chara_list_id[i] = json[StringUtils::format("chara_list_id%s", toString(i).c_str())].int_value();
                rare_card[i] = json[StringUtils::format("rare%s", toString(i).c_str())].int_value();
                chara_type[i] = json[StringUtils::format("chara_type%s", toString(i).c_str())].int_value();
                card_name_list[i] = json[StringUtils::format("name%s", toString(i).c_str())].string_value();
                HMSLOG("hit_card=%i", hit_card[i]);
            }
            _next_chara_id = toString(hit_card[0]);
            _next_chara_list_id = toString(chara_list_id[0]);
            
            //キャラリスト保存
            SaveData::saveString("user_chara_list", json["user_chara_list"].dump());
            //スキルデータの保存
            SaveData::saveString("user_active_skill_list", json["user_active_skill_list"].dump());
            SaveData::saveString("user_passive_skill_list", json["user_passive_skill_list"].dump());
            
            //ゴールド更新
            string gold = json["gold"].string_value();
            SaveData::saveString("gold", gold);
            auto header_layer = (HeaderLayer *)this->getParent()->getChildByTag(HEADER_LAYER_TAG);
            auto gold_label = (Label *)header_layer->getChildByTag(GOLD_LABEL_TAG);
            gold_label->setString(gold);
            
            //遷移
            auto normal_scene = (NormalScene *)this->getParent();
            
            int measure_id = TimeMeasure::start() ;
            normal_scene->updateUserCharaDataByUserDefault() ;
            screenTransition();//画面遷移処理
            TimeMeasure::endAndPrint(measure_id) ;
        }
        else //不正
        {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3",false);
            auto popup_layer = popupNoChangePageLayer::create("進化に失敗しました！");
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->addChild(popup_layer, 1000);
            normal_scene->removeChildByTag(SHADOW_TAG);
            this->removeFromParent();
            HMSLOG("進化失敗");
        }
    }
}

void CharaEvolutionConfirmLayer::screenTransition(){
    //画面遷移処理
    auto normal_scene = (NormalScene *)this->getParent();
    
    auto load =(HeaderLayer *)normal_scene->getChildByTag(HEADER_LAYER_TAG);//ヘッダー情報取得
    
    //アニメーション呼出
    auto animation = EvolutionAnimationLayer::create();
    animation->setData(_chara_id, _next_chara_id, _next_chara_list_id);
    animation->setPosition(Point(0, 0));
    normal_scene->addChild(animation, 11);
    //normal_scene->removeChildByTag(MAIN_LAYER_TAG); //メインレイヤー削除
    //normal_scene->removeChildByTag(SHADOW_TAG);     //シャドーレイヤー削除
    //normal_scene->changePage("CharaEvolutionAnimation");
    
    /*
    int _gacha_type = 1;
    //ガチャリザルトの生成
    auto resultLayer = EvolutionResultLayer::create();
    resultLayer->setData(_gacha_type,1,hit_card,chara_list_id,rare_card,card_name_list,chara_type);
    resultLayer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
    resultLayer->setTag(MAIN_LAYER_TAG); //メインレイヤーに定義
    normal_scene->addChild(resultLayer,ZORDER_MAIN_LAYER,MAIN_LAYER_TAG);
    normal_scene->setName("EvolutionResult");
    this->removeFromParent();
     */
}
