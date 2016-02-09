#include "ChatBaseUpperLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "PopupOkButtonLayer.h"
#include "MapScene.h"
#include "ChatListLayer.h"

ChatBaseUpperLayer::ChatBaseUpperLayer()
:_chat_type("")
,_enable_submit(true)
,_just_before_arrow_tap_flg(false)
{
}

ChatBaseUpperLayer* ChatBaseUpperLayer::create(string scene_name)
{
    ChatBaseUpperLayer *node = new ChatBaseUpperLayer();
    if (node && node->init(scene_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool ChatBaseUpperLayer::init(string scene_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ChatBaseUpperLayer::init()");
    
    ratio = getVisibleRatio();
    
    _scene_name = scene_name;
    
    return true;
}

void ChatBaseUpperLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景
    auto shadow = Sprite::create("init/Home/Common/back_shadow70.png");
    shadow->setScale(ratio);
    shadow->setAnchorPoint(Point(0.5, 0));
    shadow->setPosition(convertFromDisplaySize(Point(320, 960), 0.5, 1));
    this->addChild(shadow, 1);

    //各種ボタン配置
    _img_arrow = Sprite::create("init/Team/chat/bt_chat_arrow.png");
    _img_arrow->setScale(ratio);
    _img_arrow->setPosition(convertFromDisplaySize(Point(560, 944), 0.5, 1));
    this->addChild(_img_arrow, Z_CHAT_ARROW, T_CHAT_ARROW);
    
    _img_all = Sprite::create("init/Team/chat/bt_chat_all.png");
    _img_all->setScale(ratio);
    _img_all->setPosition(convertFromDisplaySize(Point(120, 944), 0.5, 1));
    this->addChild(_img_all, Z_CHAT_ARROW, T_CHAT_ARROW);
    
    _img_team = Sprite::create("init/Team/chat/bt_chat_team.png");
    _img_team->setScale(ratio);
    _img_team->setPosition(convertFromDisplaySize(Point(360, 944), 0.5, 1));
    this->addChild(_img_team, Z_CHAT_ARROW, T_CHAT_ARROW);
    
    createNewMark();
    
    //入力系
    //入力ボックス背景
    Sprite *img = Sprite::create("init/Team/chat/bar2.png");
    img->setAnchorPoint(Point(0.5, 0.5)) ;
    img->setPosition(convertFromDisplaySize(Point(320, 40+960), 0.5, 1));
    img->setScale(ratio);
    this->addChild(img, Z_CHAT_INPUT);
    
    //送信ボタン
    _img_submit = Sprite::create("init/Team/chat/transmission.png");
    _img_submit->setAnchorPoint(Point(0.5, 0.5)) ;
    _img_submit->setPosition(convertFromDisplaySize(Point(580, 40+960), 0.5, 1));
    _img_submit->setScale(getVisibleRatio()*0.8);
    this->addChild(_img_submit, Z_CHAT_INPUT, T_CHAT_INPUT);
    
    //入力ボックス
    Size box_size = Size(518*getVisibleRatio(), 56*getVisibleRatio());
    ui::EditBox *box = ui::EditBox::create(box_size, ui::Scale9Sprite::create("init/Team/chat/box.png"));
    box->setAnchorPoint(Point(0.5, 0.5)) ;
    box->setPosition(convertFromDisplaySize(Point(264, 40+960), 0.5, 1));
    box->setInputMode(ui::EditBox::InputMode::ANY);
    box->setFontColor(Color4B::BLACK);
    box->setMaxLength(40);
    box->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    box->setTag(1);
    this->addChild(box, Z_CHAT_INPUT, T_CHAT_EDIT);
    
    //変数設定
    _arrow_height = 32;
    Size visibleSize = Director::getInstance()->getVisibleSize();
    _y_highest = 0;
    _y_lowest = -visibleSize.height + _arrow_height*ratio;//32は矢印ボタンの高さ
    if(_scene_name=="NormalScene") _y_lowest += FOOTER_SIZE*ratio;
    
    //レイヤー位置を読み込み
    int y = SaveData::loadInt("chat_position_y") ;
    if(y<=_y_highest && y>_y_lowest) {
        this->setPosition(Point(0, y)) ;
    }

    //選択中のチャットを示す画像
    Sprite *select = Sprite::create("init/Team/chat/bt_chat_select.png");
    select->setScaleY(ratio*0.9);
    select->setScaleX(ratio);
    this->addChild(select, Z_CHAT_SELECT, T_CHAT_SELECT) ;
   
    //chat_typeはチームに所属している場合のデフォルトはteam していない場合はall
    if(SaveData::loadInt("team_id")==0){
        switchChatType("all") ;
    }else {
        switchChatType("team") ;
    }
}

void ChatBaseUpperLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ChatBaseUpperLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ChatBaseUpperLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ChatBaseUpperLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ChatBaseUpperLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ChatBaseUpperLayer::httpTeamCommentContributeRequest(string comment)
{
    string url = HOST;
    url += "team/team_comment_contribute.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&team_id=" + toString(SaveData::loadInt("team_id"));
    url += "&chat_type=" + _chat_type ;
    url += "&comment=";
    url += URLEncode(comment);
    
    HttpConnectHms::sendHttpRequest(url, [this,comment](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response){
        if(response->isSucceed()){
            httpTeamCommentContributeCompleted(sender, response);
        }else{//送信失敗したら時間おいて再送信
            this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this,comment](){
                httpTeamCommentContributeRequest(comment);
            }), NULL));
        }
    });
}


void ChatBaseUpperLayer::httpTeamCommentContributeCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    //通信エラー以外
    //if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    
    Json json = Json::parse(result, err);
    
    string error_status = json["error_status"].string_value();
    if(error_status == ""){
        httpTeamCommentRefreshRequest(0.0f) ;
    }else{
        //エラー時
        showPopup(json["error_message"].string_value());
    }
}

//チャット削除
void ChatBaseUpperLayer::httpTeamCommentDeleteCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
//    string result = HttpConnect::getHttpResponse(sender, response);
//    //通信エラー以外
//    if(result=="not connect"||result=="response NG") return;
//    
//    HMSLOG("result = %s", result.c_str());
//    
//    
//    HmsVector<string> str_vec = split(result, SPLIT_CHARACTER_HEAD);
//    
//    for(int i=0; i<str_vec.size(); i++) {
//        HMSLOG("vec[%d]=[%s]", i, str_vec.at(i).c_str());
//    }
//    
//    NormalScene *normal_scene = (NormalScene*)this->getParent();
//    
//    //エラー
//    if(str_vec.at(0)=="error") {
//        //メッセージ表示
//        showPopup("予期せぬエラーが発生しました。");
//        recreateChatLayer() ;
//    }
//    //存在しないコメント
//    else if(str_vec.at(0)=="target_not") {
//        //メッセージ表示
//        showPopup("存在しないコメントです。");
//
//        SaveData::saveString("team_chat_data", str_vec.at(1));
//        recreateChatLayer() ;
//    }
//    //削除権限の無いコメント
//    else if(str_vec.at(0)=="not_admin") {
//        //メッセージ表示
//        showPopup("予期せぬエラーが発生しました。");
//
//        recreateChatLayer() ;
//    }
//    //成功
//    else if(str_vec.at(0)=="success") {
//        SaveData::saveString("team_chat_data", str_vec.at(1));
//        recreateChatLayer() ;
//    }
}


//チャット更新
void ChatBaseUpperLayer::httpTeamCommentRefreshRequest(float dt)
{
    if(HttpConnect::_connect_flg){
        return ;
    }
    
    //更新リクエスト
    string url = HOST;
    url += "team/team_comment_refresh_revision.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&team_id=" + toString(SaveData::loadInt("team_id"));
    url += "&chat_type=" + _chat_type ;
    
    HttpConnectHms::sendHttpRequest(url, [this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response){
        if(response->isSucceed()){
            //if(isRunning()){//このオブジェクトが実行中かどうか これが無いとダンジョンシーンに移った時にオブジェクト無いのにここに入って落ちる
                httpTeamCommentRefreshCompleted(sender, response);
            //}else{
                
            //}
        }else{
            runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
                httpTeamCommentRefreshRequest(0.0f);
            }), NULL));
        }
    });
}

//チャット更新コールバック
void ChatBaseUpperLayer::httpTeamCommentRefreshCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    SaveData::saveString("team_chat_data_revision", json["team_comment_list"].dump());
    SaveData::saveString("chat_type", _chat_type);
    
    //最新のチャットID（既読のチャットIDを消すため）
    int min_chat_id = json["min_chat_id"].int_value();
    
    string key_name = "team_chat_id_list_" + _chat_type;
    string team_chat_id_list = SaveData::loadString(key_name.c_str());
    if(team_chat_id_list!="") {
        string str = "";
        string delim = "";
        HmsVector<string> id_list = split(team_chat_id_list, ":");
        for(int i=0; i<id_list.size(); i++) {
            int id = fromString<int>(id_list.at(i));
            if(id>=min_chat_id) {
                str += delim + toString(id);
                delim = ":";
            }
        }
        SaveData::saveString(key_name.c_str(), team_chat_id_list);
    }
    
    if(_chat_type=="all"){
        if(json["enable_submit_all"].bool_value()){
            setEnableSubmit(true) ;
        }else{
            setEnableSubmit(false) ;
        }
    }
    
    recreateChatLayer(json["user_data_list"]) ;
    
    float next_request_time ;
    if(checkChatLayerClose()){
        next_request_time = json["next_request_time_long"].int_value();
    }else {
        next_request_time = json["next_request_time_short"].int_value();
    }
    
    //次の通信の時間を指定してrequestを継続して行う
    //this->scheduleOnce(schedule_selector(ChatBaseUpperLayer::httpTeamCommentRefreshRequest), 0.3f);
    runAction(Sequence::create(DelayTime::create(next_request_time),CallFunc::create([this](){//少し時間をおき
        httpTeamCommentRefreshRequest(0.0f);
    }), NULL));
}

bool ChatBaseUpperLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ChatBaseUpperLayer!");
    
    int y = this->getPosition().y;
    Point tap_point = touch->getLocation();
    if(_img_arrow->boundingBox().containsPoint(Point(tap_point.x, tap_point.y - y))) {
        
        if(checkDoubleTap()){
            if(checkChatLayerClose()){
                //ダブルタップの時、チャットがしまわれていったら半分まで開く
                showHalfChatLayer() ;
                if(_chat_type == "team")dispawnNewMark();
            }else{
                //ダブルタップの時、チャットが開いていたらしまう
                hideChatLayer();
            }
            
            return true;
        }
        
        HMSLOG("arrow");
        
        _tap_flg = true;
        _tap_point = touch->getLocation();
        
        return true;
        
    }
    
    if(Utility::containsPoint(_img_all, touch->getLocation())) {
        switchChatType("all") ;
        return true;
    }
    
    if(Utility::containsPoint(_img_team, touch->getLocation())) {
        if(SaveData::loadInt("team_id") == 0){
            showPopup("チームに所属して下さい。");
        }else{
            switchChatType("team") ;
            dispawnNewMark();
        }
        return true;
    }
    
    if(checkPositionContainChatPlace(tap_point) == false){
        return false ;
    }
    
    return true;
}

//チャットの位置に含まれるか
bool ChatBaseUpperLayer::checkPositionContainChatPlace(Point tap_point){
    Size visibleSize = Director::getInstance()->getVisibleSize();
    int hidden_y = visibleSize.height + this->getPosition().y;
    HMSLOG("tap_point.y=%f, hidden_y=%d, layer_y=%f", tap_point.y, hidden_y, this->getPosition().y);
    
    if(tap_point.y < hidden_y) {
        return false;
    }else{
        return true ;
    }
}

bool ChatBaseUpperLayer::checkDoubleTap(){
    if(_just_before_arrow_tap_flg){
        //0.3秒以内にタップされていた場合、ダブルタップとみなす
        _just_before_arrow_tap_flg=false ;
        this->unschedule(schedule_selector(ChatBaseUpperLayer::updateDoubleTapCheck)) ;
        return true;
    }else{
        //この処理が呼ばれた時_just_before_arrow_tap_flgがtrueになり、0.3秒後にfalseになる。
        _just_before_arrow_tap_flg = true ;
        this->scheduleOnce(schedule_selector(ChatBaseUpperLayer::updateDoubleTapCheck), 0.3f);
        return false;
    }
    
}

void ChatBaseUpperLayer::updateDoubleTapCheck(float dt){
    _just_before_arrow_tap_flg = false ;
}

void ChatBaseUpperLayer::switchChatType(string chat_type)
{
    HMSLOG("switchChatType: %s", chat_type.c_str());
    
    //現在のタイプと同じであれば変更しない
    if(_chat_type == chat_type)return ;
    
//    this->unschedule(schedule_selector(ChatBaseUpperLayer::httpTeamCommentRefreshRequest)) ;
    _chat_type = chat_type ;
    httpTeamCommentRefreshRequest(0.0);
//    this->schedule(schedule_selector(ChatBaseUpperLayer::httpTeamCommentRefreshRequest), 5.0f);
    
    Sprite *select = (Sprite *)getChildByTag(T_CHAT_SELECT) ;
    if(select){
        if(_chat_type == "all"){
            select->setPosition(convertFromDisplaySize(Point(120, 944), 0.5, 1));
            setEnableSubmit(false) ;
        } else if (_chat_type == "team"){
            select->setPosition(convertFromDisplaySize(Point(360, 944), 0.5, 1));
            setEnableSubmit(true) ;
        }
    }
}

void ChatBaseUpperLayer::showPopup(string message){
    popupNoChangePageLayer *comment = popupNoChangePageLayer::create(message);
    comment->setPosition(Point(0,-this->getPosition().y)) ;
    this->addChild(comment, 9999);
}

void ChatBaseUpperLayer::showPopupSprite(Sprite *sprite){
    popupNoChangePageLayer *comment = popupNoChangePageLayer::create("", 100);
    comment->setPosition(Point(0,-this->getPosition().y)) ;
    comment->addChild(sprite, 3, POPUP_ACTION_TAG) ;
    this->addChild(comment, 9999);
}

void ChatBaseUpperLayer::setEnableSubmit(bool flg){
    if(flg){
        _img_submit->setColor(Color3B(255, 255, 255));
        _enable_submit=true ;
    }else{
        _img_submit->setColor(Color3B(100, 100, 100));
        _enable_submit=false ;
    }
}

void ChatBaseUpperLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_tap_flg==true) {
        Point p1 = this->getPosition();
        float diff = touch->getLocation().y - _tap_point.y;
        Point p2 = Point(p1.x, p1.y+diff);
        if(p2.y<=_y_highest && p2.y>_y_lowest) {
            this->setPosition(p2);
            _tap_point = touch->getLocation();
            
            //if(_chat_type == "team")dispawnNewMark();
        }
    }
}

void ChatBaseUpperLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _tap_flg = false;
    
    int y = this->getPosition().y;
    SaveData::saveInt("chat_position_y", y) ;
    Point tap_point = touch->getLocation();
    if(_img_submit->boundingBox().containsPoint(Point(tap_point.x, tap_point.y - y))) {
        
        
        HMSLOG("submit");
        
        ui::EditBox *editBox = (ui::EditBox *)this->getChildByTag(T_CHAT_EDIT);
        _input_chat_comment = editBox->getText();
        
        //入力禁止文字チェック用
        int forbid_index = _input_chat_comment.find(ILLEGAL_CHAR);
    
        //入力禁止文字チェック
        
        if(_enable_submit == false){
            showPopup("前回の投稿から3分間は\n投稿できません。") ;
        } else if(_input_chat_comment=="") {
            //空白時は何もしない
        } else if(EditBoxUtil::containInvalidChar(_input_chat_comment)){
            string text = "無効な文字列が含まれます";
            text += "\n\n無効文字一覧  " + EditBoxUtil::getInvalidCharsString(" ");
            text += "\nその他、スペース等の空白" ;
            showPopup(text) ;
        } else {
            if(_chat_type=="all"){
                //ポップアップ表示
                auto ok_func = [this, editBox](){
                    httpTeamCommentContributeRequest(_input_chat_comment) ;
                    editBox->setText("") ;
                    setEnableSubmit(false) ;
                };
                
                auto ng_func = [](){}; //いいえを押したとき何もしない
                popupConfirmLayer *layer = popupConfirmLayer::create("全体チャットに投稿すると\n全ユーザーに閲覧されます。\nまた、一度発言すると\n３分間投稿できません。");
                layer->setPosition(Point(0,-this->getPosition().y)) ;
                layer->setFunction(ok_func, ng_func);
                addChild(layer, 999999);
            }else if(_chat_type=="team"){
                httpTeamCommentContributeRequest(_input_chat_comment) ;
                editBox->setText("") ;
            }
        }

        return;
    }
}

void ChatBaseUpperLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ChatBaseUpperLayer::recreateChatLayer(Json user_data_list)
{
    //直近に生成したチャット内容と同じだったら 新たにレイヤーを生成しない
    string team_chat_data_json_dump = SaveData::loadString("team_chat_data_revision") ;
    if(team_chat_data_json_dump == _pre_team_chat_data_json_dump){
        return ;
    }
    
    //    チャット更新
    ChatListLayer *_chat_list_layer = ChatListLayer::create(user_data_list);
    
    removeChildByTag(T_CHAT_SCROLL) ;
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, 78), 0.0, 0));
    if(_scene_name=="NormalScene"){
        pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (FOOTER_SIZE * ratio)));
    }else{
        pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height ));
    }
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, Z_CHAT_SCROLL, T_CHAT_SCROLL);
    
    
    pScrollView->setContainer(_chat_list_layer);
    
    float scrollview_contents_size_y = _chat_list_layer->getScrollHeight();
    
    //scrollのViewSizeの高さがコンテンツサイズが満たない場合、Viewの大きさにそろえる (ScrollLayerはコンテンツサイズがViewの範囲より小さいと挙動が変)
    if(scrollview_contents_size_y < pScrollView->getViewSize().height){
        scrollview_contents_size_y = pScrollView->getViewSize().height  ;
    }
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, scrollview_contents_size_y));
    
    pScrollView->setPosition(convertFromDisplaySize(Point(0, 960), 0.0, 1));
    
    //未読件数表示
    int unreadChatCount = _chat_list_layer->getUnreadCount();
    if(unreadChatCount > 0 && _chat_type == "team"){
        spawnNewMark(unreadChatCount);
    }
    
    //今回のチャット内容を保存
    _pre_team_chat_data_json_dump = SaveData::loadString("team_chat_data_revision");
}

void ChatBaseUpperLayer::createNewMark(){
    _img_new_mark = Sprite::create("init/Home/Mypage/bt_mypage_red_circle.png");
    _img_new_mark->setScale(0);
    //_img_new_mark->setPosition(convertFromDisplaySize(Point(420, 944), 0.5, 1));
    _img_new_mark->setPosition(convertFromDisplaySize(Point(290, 944), 0.5, 1));
    this->addChild(_img_new_mark, Z_CHAT_RED, T_CHAT_RED);
    
    _mark_number_label = HMSFontUtil::createWithTTFBySystemFontArgument("0", FONT_NAME_SYSTEM, 24);
    _mark_number_label->setPosition(Point(_img_new_mark->getContentSize() / 2));
    _img_new_mark->addChild(_mark_number_label, ZORDER_BUTTON);
}

void ChatBaseUpperLayer::spawnNewMark(int number){
    if(_img_new_mark->getScale() == 1)  return;
    if(number > 99) number = 99;
    _mark_number_label->setString(toString<int>(number));
    _img_new_mark->setScale(0);
    ScaleTo *scaleToBig = ScaleTo::create(0.5f, ratio * MARK_SIZE) ;
    EaseElasticOut *scaleEaseOut = EaseElasticOut::create(scaleToBig);
    _img_new_mark->runAction(scaleEaseOut);
    
}

void ChatBaseUpperLayer::dispawnNewMark(){
    if(_img_new_mark->getScale() == 0)  return;

    ScaleTo *scaleToSmall = ScaleTo::create(0.5f, 0) ;
    EaseElasticOut *scaleEaseOut = EaseElasticOut::create(scaleToSmall);
    CallFunc *func = CallFunc::create([this](){
        _mark_number_label->setString("0");
    });
    Sequence* seq = Sequence::create(scaleEaseOut, func, nullptr);
    
    _img_new_mark->runAction(seq);
    
}

bool ChatBaseUpperLayer::checkChatLayerClose()
{
    //チャットが閉じられているか
    if(this->getPosition().y > -50){
        return true ;
    }else {
        return false ;
    }
}

void ChatBaseUpperLayer::hideChatLayer()
{
    this->stopAllActions() ;
    this->runAction(Sequence::create(MoveTo::create(0.2f, Point(0,0)),CallFunc::create([](){SaveData::saveInt("chat_position_y", 0);}),nullptr)) ;
}

void ChatBaseUpperLayer::showHalfChatLayer()
{
    this->stopAllActions() ;
    this->runAction(Sequence::create(MoveTo::create(0.2f, Point(0,-_contentSize.height/2)),CallFunc::create([this](){SaveData::saveInt("chat_position_y", -_contentSize.height/2);}),nullptr)) ;
}

void ChatBaseUpperLayer::onExit(){
    unscheduleAllCallbacks();
    stopAllActions();
    Layer::onExit();
}
