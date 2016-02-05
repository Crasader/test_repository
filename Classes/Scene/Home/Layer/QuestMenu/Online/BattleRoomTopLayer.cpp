#include "BattleRoomTopLayer.h"

bool BattleRoomTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleRoomTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BattleRoomTopLayer::createButton()
{
    //ルーム作成
    auto create_btn_off = Sprite::create("init/Home/Quest/Online/bt_room_make.png");
    auto create_btn_on  = Sprite::create("init/Home/Quest/Online/bt_room_make.png");
    create_btn_on->setColor(Color3B(180, 180, 180));
    
    auto create_btn = MenuItemSprite::create(create_btn_off, create_btn_on , [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("CreateRoom");
    });
    create_btn->setScale(ratio);
    
    //ルーム検索
    auto search_btn_off = Sprite::create("init/Home/Quest/Online/bt_room_join.png");
    auto search_btn_on  = Sprite::create("init/Home/Quest/Online/bt_room_join.png");
    search_btn_on->setColor(Color3B(180, 180, 180));
    
    auto search_btn = MenuItemSprite::create(search_btn_off, search_btn_on , [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("SearchRoom");
    });
    search_btn->setScale(ratio);

    auto menu = Menu::create(create_btn, search_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 310), 0.5, 1));
    menu->alignItemsVerticallyWithPadding(0 * ratio);
    this->addChild(menu, 2);
    
    //バナー
    Sprite *img = Sprite::create("init/Home/Quest/Online/banner_areabattle.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 625), 0.5, 1));
    this->addChild(img, 2);
    //create_btn->setPosition(convertFromDisplaySize(Point(320, 410), 0.5, 1));
    //search_btn->setPosition(convertFromDisplaySize(Point(320, 190), 0.5, 1));
    
    //ヘルプ
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/Online/bt_help_areabattle.png");
    auto detail_on  = Sprite::create("init/Home/Quest/Online/bt_help_areabattle.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__areabattle&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "エリアバトルについて");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu2 = Menu::create(detail_btn, nullptr);
    menu2->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu2, ZORDER_BUTTON);
}

void BattleRoomTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ルーム選択", "BattleTop", this);
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //ボタンの作成
    createButton();
}
