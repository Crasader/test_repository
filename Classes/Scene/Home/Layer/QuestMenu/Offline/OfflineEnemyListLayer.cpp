#include "OfflineEnemyListLayer.h"

bool OfflineEnemyListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("OfflineEnemyListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void OfflineEnemyListLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの作成
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("対戦相手選択", "QuestMenu", this);
    
    //バトルユーザー選出
    battleUserRequest();
}

void OfflineEnemyListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(OfflineEnemyListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(OfflineEnemyListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(OfflineEnemyListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(OfflineEnemyListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

//対戦相手取得リクエスト
void OfflineEnemyListLayer::battleUserRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "battle/get_offline_battle_user.php";
    auto selector = httpresponse_selector(OfflineEnemyListLayer::battleUserRequestCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//対戦相手選出レスポンス
void OfflineEnemyListLayer::battleUserRequestCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result:%s",result.c_str());
    
    string err;
    
    Json json = Json::parse(result, err);
    _battle_user_list = json["battle_user_list"].object_items();
    
    //データの数を取得
    _data_count = countJsonData(_battle_user_list);
    //    HMSLOG("_data_count = %d", _data_count);
    
    //バトルユーザースプライトの生成
    initBattleUserSprite();
    
    if (_enemy_cells.size() > 0)
    {
        //スクロールビューの作成
        createScrollView();
    }
    else
    {
        //対戦相手が見つからないとき
        string message = "対戦相手が見つかりませんでした。\n出陣メニューに戻ります。";
        auto popup_layer = popupChangePageLayer::create(message, "QuestMenu");
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void OfflineEnemyListLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, ORDER_SCROLLVIEW, SCROLLVIEW_TAG);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _data_count + _cell_size / 10));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _data_count + _cell_size / 10) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* OfflineEnemyListLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    _cell_size = 138 * ratio;
    auto layer = Layer::create();
    
    //プレゼント画像の配置
    int num = 0;
    for (int i = _data_count-1; 0 <= i; i--)
    {
        _enemy_cells[num]->setScale(ratio);
        _enemy_cells[num]->setTag(num);
        _enemy_cells[num]->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        layer->addChild(_enemy_cells[num], 1);
        num++;
    }
    
    return layer;
}

void OfflineEnemyListLayer::initBattleUserSprite()
{
    //バトルユーザー画像の初期化
    _enemy_cells.clear();
    
    //バトルユーザー画像の作成
    int json_count = countJsonData(_battle_user_list) ;
    for (int i = 0; i < json_count; i++)
    {
        //        Json json2 = jsonParseForUserInfoCellSprite(i);
        string num = toString<int>(i);
        _enemy_cells.push_back(EnemySprite::create(_battle_user_list[num].object_items()));
    }
}

void OfflineEnemyListLayer::changePage(int tag)
{
    string jsonStr = _enemy_cells[tag]->getJson() ;
    
    //デッキ編成画面でスプライトを生成するためのjson文字列を保存
    SaveData::saveString("battle_user_sprite_data", jsonStr);
    HMSLOG("battle_user_sprite_data = %s", SaveData::loadString("battle_user_sprite_data").c_str());
    
    //ダンジョンシーンで使用するデータを保存
    string err;
    Json json = Json::parse(jsonStr, err);
    string enemy_user_master_id = json["user_master_id"].string_value() ;
    saveUseDungeonData(enemy_user_master_id) ;
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("BattleFriendChoiceForOffline");
}

//ダンジョンシーンで使用するデータを保存
void OfflineEnemyListLayer::saveUseDungeonData(string enemy_user_master_id)
{
    //Dungeonシーンで使用する相手のuser_master_idを保存
    SaveData::saveString("enemy_user_master_id", enemy_user_master_id);
    HMSLOG("enemy_user_master_id:%s", SaveData::loadString("enemy_user_master_id").c_str());
}

bool OfflineEnemyListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_data_count > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void OfflineEnemyListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void OfflineEnemyListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                changePage(sp->getTag());
                HMSLOG("tag = %d", sp->getTag());
            }
            tag++;
        }
    }
}
void OfflineEnemyListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void OfflineEnemyListLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}