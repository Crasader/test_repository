#include "LastConfirmLayer.h"
#include "TutorialBattleScene.h"
#include "ApsalarEvent.h"
#include "Cocos2dxFox.h"

#define WINDOW_SIZE 196
#define MESSAGE_SIZE 190
bool LastConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("LastConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _user_name   = SaveData::loadString("user_name");
    _generalType = SaveData::loadString("generalType");
    
    window_default_position = convertFromDisplaySize(Point(320, 80), 0.5, 0.0) ;
    window_down_position = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, -WINDOW_SIZE), 0.5, 0.0) ;
    
    message_default_position = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, -MESSAGE_SIZE), 0.5, 0.0) ;
    message_up_position = convertFromDisplaySize(Point(320, 80), 0.5, 0.0)  ;
    
    return true;
}

void LastConfirmLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(LastConfirmLayer::onTouchBegan, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void LastConfirmLayer::createMessage()
{
    //キャラ画像作成
    message_talk_sprite = MessageTalkSprite::create() ;
    message_talk_sprite->setScale(ratio);
    message_talk_sprite->setAnchorPoint(Point(0.5, 0)) ;
    message_talk_sprite->setPosition(message_default_position);
    addChild(message_talk_sprite, 1, TAG_MESSAGE_SPRITE);
}
void LastConfirmLayer::initDeckSprite()
{
    
    /*
    string file_path ;
    
    //ファイルパス取得
    if(_generalType == "1") file_path = "download/Gacha/banner_gacha_detail1.png" ;
    else if(_generalType == "2") file_path = "download/Gacha/banner_gacha_detail1.png" ;
    else if(_generalType == "3") file_path = "download/Gacha/banner_gacha_detail1.png" ;
    else if(_generalType == "4") file_path = "download/Gacha/banner_gacha_detail1.png" ;
    else if(_generalType == "5") file_path = "download/Gacha/banner_gacha_detail1.png" ;
    
    //説明画像の作成
    Sprite *detail = Sprite::create(file_path);
    detail->setAnchorPoint(Vec2(0.5, 0.5));
    detail->setPosition(convertFromDisplaySize(Point(320, 500), 0.5, 0.5));
    detail->setScale(ratio);
    this->addChild(detail) ;
     */
    
    
    //デッキ画像
    Point deck_select_center_position = Point(320, 500) ;
    Sprite *deck = Sprite::create("init/Tutorial/deck_select_bk"+_generalType+".png");
    Sprite *chara= Sprite::create("init/Tutorial/deck_select_chara"+_generalType+".png");
    chara->setPosition(deck->getContentSize()/2) ;
    deck->addChild(chara) ;
    deck->setAnchorPoint(Vec2(0.5, 0.5));
    deck->setPosition(convertFromDisplaySize(deck_select_center_position, 0.5, 0.5));
    deck->setScale(ratio);
    this->addChild(deck) ;
    
    
    //説明画像生成
    float description_x ;
    float description_y = 350;
    if(_generalType == "1") description_x = 224 ;
    else if(_generalType == "2") description_x = 224 ;
    else if(_generalType == "3") description_x = 350 ;
    else if(_generalType == "4") description_x = 300 ;
    else if(_generalType == "5") description_x = 311 ;
    
    Sprite *description = Sprite::create("init/Tutorial/deck_select_balloon"+_generalType+".png");
    description->setScale(ratio);
    description->setPosition(convertFromDisplaySize(Point(description_x, description_y), 0.5, 0.5));
    this->addChild(description) ;
    
    //デッキ名
    Sprite *deck_name = Sprite::create("init/Tutorial/deck_select_title"+_generalType+".png");
    deck_name->setScale(ratio) ;
    deck_name->setAnchorPoint(Point(0.5, 0));
    deck_name->setPosition(convertFromDisplaySize(deck_select_center_position + Point(0, deck_name->getContentSize().height + 220), 0.5, 0.5));
    this->addChild(deck_name,2) ;
}
    
void LastConfirmLayer::createButton()
{
    //ベースとなるウィンドウを生成する(画面下端から40ピクセル)
    Sprite *window = Sprite::create("init/Tutorial/popup_select.png");
    window->setScale(ratio);
    window->setAnchorPoint(Point(0.5, 0)) ;
    window->setPosition(window_default_position);
    this->addChild(window, 3, TAG_WINDOW) ;
    
    //ラベルの生成 (下から3/4の位置が中心位置)
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument("このデッキでよろしいですか？", FONT_NAME_SYSTEM, 33);
    label->setPosition(Point(window->getContentSize().width/2, window->getContentSize().height*3/4)) ;
//enableOutline(Color4B::BLACK,1);
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    window->addChild(label, 1);
    
    //はいボタンの作成
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem1 = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender)
    {
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/onpu.mp3");

        Node *node = getChildByTag(TAG_WINDOW) ;
        Menu *menu = (Menu*)node->getChildByTag(TAG_MENU) ;
        menu->setEnabled(false) ;

        //名前と選択属性インサート通信処理
        userDataInsertRequest();
    });

    //いいえボタンの作成
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem2 = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_no.mp3");

        //名前入力ページへ戻る
        auto tutorial_scene = (TutorialScene *)this->getParent();
        auto next_layer = DeckPackSelectLayer::create();
        tutorial_scene->changePage(next_layer);
    });
    
    auto menu = Menu::create(menuItem1, menuItem2, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(window->getContentSize().width/2, 60) ;
    window->addChild(menu, 1, TAG_MENU);
}

/*
void LastConfirmLayer::showUserName()
{
    //ユーザー名の表示
    string user_text = "名前 : "+_user_name;
    auto user_name = HMSFontUtil::createWithTTFBySystemFontArgument(user_text, FONT_NAME_SYSTEM, 36);
    user_name->setPosition(Point(130, 230));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    user_name->setAnchorPoint(Point(0, 0.5));
    popup_bg->addChild(user_name, 1);
}
 */

//通信処理
void LastConfirmLayer::userDataInsertRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "tutorial/tutorial_insert.php";
    auto selector = httpresponse_selector(LastConfirmLayer::userDataInsertCallback);
    string body = "user_master_id="+user_master_id+"&user_name="+_user_name+"&deck_type="+_generalType;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
void LastConfirmLayer::userDataInsertCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["user_name_insert_flg"].bool_value() && json["card_insert_flg"].bool_value())
    {

        //チュートリアルステータスの変更
        SaveData::saveInt("tutorialStatus", 3);
        
        //FOX LTV計測地点（チュートリアル完了）
        if(RAKUTEN_FLG){
            //楽天の場合計測無し
        } else if (IF_IOS) {
            FoxPlugin::sendLtv(6423);
        } else if (IF_ANDROID) {
            FoxPlugin::sendLtv(6425);
        }
        
        nextStep(0.0f) ;
        
    }
}

bool LastConfirmLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(step_state!=0){
        if(message_talk_sprite->isTalkWait()){
        }else if(message_talk_sprite->isTalkNow()){
            //トーク中にタップした場合全てのコメントを強制的に表示する
            message_talk_sprite->forceAllTalk() ;
        }else if(message_talk_sprite->isTalkEnd()){
            //最後のメッセージのとき以外メッセージをWait状態にする
            if(step_state != 2)message_talk_sprite->talkWait() ;
            nextStep(0.0f);
        }
    }
    return true;
}

void LastConfirmLayer::nextStep(float dt){
    
    if(step_state == 0){
        //windowとちょうせんの位置を交換
        exchageWindowForMessage() ;
    }else if(step_state == 1){
        message_talk_sprite->talkStart("それでは、三国志バトルウォーズを\nお楽しみください！") ;
        step_state++ ;
    }else if(step_state == 2){
        changeSceneToTitleScene() ;
        step_state++ ;
    }
    
}

void LastConfirmLayer::exchageWindowForMessage(){
    
    Sprite *window = (Sprite *)getChildByTag(TAG_WINDOW) ;
    
    MoveTo *window_move = MoveTo::create(0.2f, window_down_position) ;
    MoveTo *message_move = MoveTo::create(1.0f, message_up_position) ;
    
    auto step_add_func = CallFunc::create([this](){
        message_talk_sprite->talkStart("このデッキを選ぶなんて、\nお目が高いですね！") ;
        step_state++ ;
    });
    
    window->runAction(window_move) ;
    message_talk_sprite->runAction(Sequence::create(DelayTime::create(0.4f), message_move, step_add_func, nullptr)) ;
    
}

void LastConfirmLayer::changeSceneToTitleScene() {
    //真っ白なスプライトを画面いっぱいにかぶせる
    auto spr = Sprite::create();
    spr->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    spr->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    spr->setColor(Color3B::WHITE);
    spr->setOpacity(0) ;
    addChild(spr,99999);
    
    FadeIn *fade = FadeIn::create(2) ;
    
    auto go_title_func = CallFunc::create([this](){
        //タイトルシーンへの遷移
        auto tutorial_scene = (TutorialScene *)this->getParent();
        tutorial_scene->unscheduleAllCallbacks() ;
        
        auto scene = TitleScene::createScene();
        auto transition = TransitionFade::create(3.0f, scene);
        Director::getInstance()->replaceScene(transition);
    });
    
    spr->runAction(Sequence::createWithTwoActions(fade, go_title_func));
}

void LastConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //メッセージの作成
    createMessage();
    
    //説明画像初期化
    initDeckSprite();
    
    //ボタンの作成
    createButton();
    
    //イベントリスナーの登録
    createEventLitener() ;
}
