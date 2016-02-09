#include "ReplayListLayer.h"
#include "PopupConfirmLayer.h"
#include "ReplayScene.h"

#define SWITCH_TAB_TAG 11
#define LIST_TAG 101

bool ReplayListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ReplayListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void ReplayListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(ReplayListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ReplayListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ReplayListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ReplayListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ReplayListLayer::getReplayList()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "battle/get_replay_list.php?user_master_id=" + user_master_id;
    url += "&type=" + type;
    
    //auto http_connect = HttpConnect::create();
    auto selector = httpresponse_selector(ReplayListLayer::getReplayListCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void ReplayListLayer::getReplayListCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("response:%s", str.c_str());
    //通信エラー以外
    if(str != "not connect" && str != "response NG") {
        string err;
        json_master = Json::parse(str, err);
        
        json_count = countJsonData(json_master);
        
        if(json_count>0) {
            createScrollView();
        } else {
            //データがなかった時の処理
            string message = "データがありません。";
            auto popup_layer = popupNoChangePageLayer::create(message);
            addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
            
            createScrollView();

        }
    }
}

void ReplayListLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio)- (FOOTER_SIZE * ratio) - 60*ratio));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 2, SCROLLVIEW_TAG);
    
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * json_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * json_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio + 60*ratio)))));
    
    //ボタンなど
    Sprite *img;
    if(type=="all") {
        img = Sprite::create("init/Home/Quest/replay_tab_myself_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+2);

        img = Sprite::create("init/Home/Quest/replay_tab_favorite_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(520, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+3);
        
        img = Sprite::create("init/Home/Quest/replay_tab_all.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 286), 0.5, 1));
        this->addChild(img, 1, SWITCH_TAB_TAG);
    } else if(type=="myself") {
        img = Sprite::create("init/Home/Quest/replay_tab_all_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(120, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+1);

        img = Sprite::create("init/Home/Quest/replay_tab_favorite_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(520, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+3);
        
        img = Sprite::create("init/Home/Quest/replay_tab_myself.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 286), 0.5, 1));
        this->addChild(img, 1, SWITCH_TAB_TAG);
    } else {
        img = Sprite::create("init/Home/Quest/replay_tab_all_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(110, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+1);
        
        img = Sprite::create("init/Home/Quest/replay_tab_myself_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 716), 0.5, 1));
        this->addChild(img, 0, SWITCH_TAB_TAG+2);
        
        img = Sprite::create("init/Home/Quest/replay_tab_favorite.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 286), 0.5, 1));
        this->addChild(img, 1, SWITCH_TAB_TAG);
    }
    
//    if(COCOS2D_DEBUG){//デバッグ時のみの処理
//        CCLOG("デバッグ中です");
//        ui::EditBox* editBox;
//        Size editBoxSize = Size(200, 80);
//        editBox = EditBox::create(editBoxSize, Scale9Sprite::create(IMG_TEXTBOX_BACK));
//        editBox->setPosition(convertFromDisplaySize(Point(500, 780), 0.5, 1));
//        editBox->setFontColor(Color3B::WHITE);
//        editBox->setFont(FONT_NAME_SYSTEM, 36);
//        editBox->setTag(EDITBOX_TAG);
//        //editBox->setText(SaveData::loadString("user_name").c_str());
//        editBox->setInputMode(cocos2d::extension::EditBox::InputMode::ANY);
//        editBox->setReturnType(EditBox::KeyboardReturnType::DONE);
//        addChild(editBox,5);
//        
//        
//        Sprite *temp = Sprite::create("init/Home/Quest/bt_playback.png");
//        temp->setScale(0.5f*ratio, 1.0f*ratio);
//        auto item_temp = MenuItemSprite::create(temp, temp, [this,editBox](Ref *sender){
//            
//            auto func = [this,editBox](){
//                
//                //キャンセル音
//                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
//                
//                SaveData::saveString("battle_list_id_for_replay", editBox->getText());
//                auto scene = ReplayScene::createScene();
//                auto director = Director::getInstance();
//                director->replaceScene(scene);
//            };
//            string text2 = "この対戦のリプレイを見ますか？";
//            string text3 = editBox->getText();
//            popupConfirmLayer *layer = popupConfirmLayer::create(text2+text3);
//            layer->setFunction(func, nullptr);
//            this->addChild(layer, 2);
//            return;
//            
//            
//        });
//        auto *menu = Menu::create(item_temp, NULL);
//        menu->setPosition(convertFromDisplaySize(Point(550, 780), 0.5, 1));
//        addChild(menu,100);
//    }
    
}

bool ReplayListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //ボタンタッチフラグtrue
    touchButtonFlg = true;
    
    return true;
}

Layer *ReplayListLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 130 * ratio;
    auto layer = Layer::create();
    
    for(int i=json_count-1; 0<=i; i--) {
        string num = toString<int>(json_count - 1 -i);

        string err;
        Json json = json_master[toString(i)].object_items();
        HMSLOG("data_text=%s", json["data_text"].dump().c_str());

        Json json_data = Json::parse(json["data_text"].string_value(), err);
        string battle_list_id = json["battle_list_id"].string_value();
        string user_master_id1 = json["user_master_id1"].string_value();
        string user_master_id2 = json["user_master_id2"].string_value();
        string user_master_id = SaveData::loadString("user_master_id");
        //HMSLOG("json=%s", json.dump().c_str());
        HMSLOG("battle_list_id=%s, u=%s, u1=%s, u2=%s", battle_list_id.c_str(), user_master_id.c_str(), user_master_id1.c_str(), user_master_id2.c_str());
        battle_list_id_list.insert(battle_list_id);
        
        HMSLOG("player_data1=%s", json_data["player_data1"].dump().c_str());
        HMSLOG("player_data2=%s", json_data["player_data2"].dump().c_str());

        Json json_player1 = Json::parse(json_data["player_data1"].dump(), err);
        Json json_player2 = Json::parse(json_data["player_data2"].dump(), err);
        //Json json_leader_card = Json::parse(json_player["card" + to_string(1)].dump(), err);
        
        //背景
        Sprite *bg = Sprite::create("init/Home/Quest/list_replay.png");
        bg->setScale(ratio);
        bg->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
        layer->addChild(bg, 1, LIST_TAG+json_count-1-i);
        
        //リプレイボタン
        bg = Sprite::create("init/Home/Quest/bt_playback.png");
        bg->setScale(ratio);
        bg->setPosition(Point(547 * ratio, (cell_size * i) + (cell_size / 2)));
        layer->addChild(bg, 1);
        
        if(type=="all") {
            Json json_leader_card1 = Json::parse(json_player1["card"][0].dump(), err);
            int id1 = json_leader_card1["id"].int_value();
            string chara_id1 = toString(id1);
            Json json_leader_card2 = Json::parse(json_player2["card"][0].dump(), err);
            int id2 = json_leader_card2["id"].int_value();
            string chara_id2 = toString(id2);
            
            //VS,WIN,LOSE
            bg = Sprite::create("init/Home/Quest/replay_vs.png");
            bg->setScale(ratio);
            bg->setPosition(Point(242 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
            
            string result_type1 = json["result_type"].string_value();
            string result_type2;
            if(result_type1=="win") result_type2 = "lose";
            else result_type2 = "win";
            
            bg = Sprite::create("init/Home/Quest/replay_" + result_type1 + ".png");
            bg->setScale(ratio);
            bg->setPosition(Point(166 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
            
            bg = Sprite::create("init/Home/Quest/replay_" + result_type2 + ".png");
            bg->setScale(ratio);
            bg->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
            
            //キャラ
            string card_pass = "download/Chara/thumb100/" + chara_id1 + ".png";
            bg = Sprite::create(card_pass);
            bg->setScale(ratio);
            bg->setPosition(Point(71 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
            
            card_pass = "download/Chara/thumb100/1001.png";
            bg = Sprite::create(card_pass);
            bg->setScale(ratio);
            bg->setPosition(Point(412 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
        } else {
            //対戦相手
            Label *message_label = HMSFontUtil::createWithTTFBySystemFontArgument("対戦相手", FONT_NAME_SYSTEM, 20);
            message_label->setAnchorPoint(Point(0, 0.5));
            message_label->setPosition(Point(138 * ratio, (cell_size * i) + (cell_size / 2) + 21*ratio));
			message_label->setScale(ratio);
            layer->addChild(message_label, 1);
            
            Json json_player;
            if(user_master_id1==user_master_id) {
                json_player = json_player2;
            } else {
                json_player = json_player1;
            }
            string player_name = json_player["player_name"].string_value();
            Json json_leader_card = Json::parse(json_player["card"][0].dump(), err);
            int id = json_leader_card["id"].int_value();
            string chara_id = toString(id);
            
            message_label = HMSFontUtil::createWithTTFBySystemFontArgument(player_name, FONT_NAME_SYSTEM, 33);
            message_label->setAnchorPoint(Point(0, 0.5));
            message_label->setPosition(Point(138 * ratio, (cell_size * i) + (cell_size / 2) - 11 *ratio));
			message_label->setScale(ratio);
            layer->addChild(message_label, 1);

            //VS,WIN,LOSE
            string result_type = json["result_type"].string_value();
            bg = Sprite::create("init/Home/Quest/replay_" + result_type + ".png");
            bg->setScale(ratio);
            bg->setPosition(Point(412 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);
            
            //キャラ
            string card_pass = "download/Chara/thumb100/" + chara_id + ".png";
            bg = Sprite::create(card_pass);
            bg->setScale(ratio);
            bg->setPosition(Point(71 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(bg, 1);

        }
    }
    
    return layer;
}

void ReplayListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ReplayListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(touchButtonFlg==true) {
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        
        //切り替えボタン判定
        for(int i=1; i<=3; i++) {
        Sprite *img = (Sprite *)this->getChildByTag(SWITCH_TAB_TAG+i);
            if(img!=nullptr) {
                Rect rect = img->getBoundingBox();
                if(rect.containsPoint(touch->getLocation())) {
                    CCLOG("switch");
                    
                    //効果音
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/normal_tap.mp3",false);
                    
                    if(i==1) type = "all";
                    else if(i==2) type = "myself";
                    else type = "favorite";
                    
                    //this->removeAllChildrenWithCleanup(true);
                    //HeaderLayer::setupTitleBar("対戦リプレイ", "BattleTop", this);
                    this->removeChildByTag(SCROLLVIEW_TAG);
                    this->removeChildByTag(SWITCH_TAB_TAG);
                    this->removeChildByTag(SWITCH_TAB_TAG+1);
                    this->removeChildByTag(SWITCH_TAB_TAG+2);
                    this->removeChildByTag(SWITCH_TAB_TAG+3);
                    
                    battle_list_id_list.clear();
                    getReplayList();
                    
                    return;
                }
            }
        }
        
        //リスト判定
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        for(auto child : children) {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation())) {
                int index = sp->getTag() - LIST_TAG;
                
                string battle_list_id = battle_list_id_list[index];
                HMSLOG("index=%d, battle_list_id=%s", index, battle_list_id.c_str());
                
                auto func = [this, battle_list_id](){
                    HMSLOG("replay start. battle_list_id=%s", battle_list_id.c_str());
                    
                    //キャンセル音
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
                    
                    SaveData::saveString("battle_list_id_for_replay", battle_list_id);
                    auto scene = ReplayScene::createScene();
                    auto director = Director::getInstance();
                    director->replaceScene(scene);
                };
                
                popupConfirmLayer *layer = popupConfirmLayer::create("この対戦のリプレイを見ますか？");
                layer->setFunction(func, nullptr);
                this->addChild(layer, 2);
                return;
            }
        }
    }
}

void ReplayListLayer::replayStart()
{
    
}

void ReplayListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ReplayListLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

void ReplayListLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("対戦リプレイ", "BattleTop", this);

    //イベントリスナーの登録
    initEventListener();
    
    //リプレイ一覧の取得
    getReplayList();
}
