#include "ChatListLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "cocos-ext.h"
#include "NormalScene.h"
#include "ChatBaseUpperLayer.h"
#include "popupConfirmLayer.h"

USING_NS_CC_EXT;


ChatListLayer* ChatListLayer::create(Json json_user_data_list)
{
    ChatListLayer *node = new ChatListLayer();
    if (node && node->init(json_user_data_list))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool ChatListLayer::init(Json json_user_date_list)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ChatListLayer::init()");
    
    _json_user_data_list = json_user_date_list ;
    ratio = getVisibleRatio();
    _frame_list.clear();
    _user_master_id_list.clear();
    _chat_type = SaveData::loadString("chat_type");
    _unread_count = 0;
    return true;
}

void ChatListLayer::onEnter()
{
    Layer::onEnter();
    
    setup();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
//    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ChatListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ChatListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ChatListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ChatListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

ui::Scale9Sprite *ChatListLayer::createMessageCell(string comment, string post_user_type, float now_height, string image_path, string user_name, string insert_date, bool found_flg)
{
    //コメントのサイズ(px)
    int comment_font_size = 24;
    
    ////コメントの範囲の決定
    Size dimensions = calcDimensions(comment, comment_font_size) ;
    
    ////コメント////
    Label* comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(comment, FONT_NAME_SYSTEM, comment_font_size);
    comment_label->enableOutline(Color4B(255, 255, 255, 255), 1 ) ;
    comment_label->setTextColor(Color4B::BLACK);
    
    comment_label->setAnchorPoint(Point(0, 0.5));
	comment_label->setVerticalAlignment(TextVAlignment::CENTER);
	comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setDimensions(dimensions.width , dimensions.height);
    
    ////枠////
    //枠色
    string flame_file = "";
    if(post_user_type=="mine") flame_file = "init/Team/chat/comment_box_myself.png";
    else if(post_user_type == "others")flame_file = "init/Team/chat/comment_box_others.png";
    else if(post_user_type == "admin")flame_file = "init/Team/chat/comment_box_admin.png";
    ui::Scale9Sprite *frame = ui::Scale9Sprite::create(flame_file.c_str(), Rect(0, 0, 422, 64), Rect(22, 20, 378, 24));
    frame->setContentSize(comment_label->getContentSize()+Size(42, 24));
    frame->setScale(ratio);
    now_height += frame->getContentSize().height + 70;
    
    if(post_user_type=="mine") {
        frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH-110, now_height), 0.5, 0));
        frame->setAnchorPoint(Point(1, 1));
    } else if(post_user_type == "others" || post_user_type == "admin"){
        frame->setPosition(convertFromDisplaySize(Point(110, now_height), 0.5, 0));
        frame->setAnchorPoint(Point(0, 1));
    }
    
    if(post_user_type=="others" && found_flg==false) {
        //枠に赤い丸をつける
        Sprite *red = Sprite::create("init/Team/chat/mark_unread.png");
        red->setPosition(Point(frame->getContentSize().width-5*ratio, frame->getContentSize().height-5*ratio));
        frame->addChild(red, Z_RED, T_RED);
        _unread_count++;
    }
    frame->addChild(comment_label, Z_COMMENT_BOX) ;
    comment_label->setPosition(Point(19, frame->getContentSize().height/2)) ;

    ////サムネ////
    Sprite *thum = Sprite::create(image_path.c_str());
    float size = thum->getContentSize().width ;
    if(size < thum->getContentSize().height) size = thum->getContentSize().height ;
    float scale100_rate = size / 100  ; //この値をスケールの倍率にすると100になるよ
    thum->setScale(0.96*scale100_rate); //100になるサイズから0.96倍するよ

    //右寄せ、左寄せ
    if(post_user_type=="mine") {
        thum->setPosition(Point(frame->getContentSize().width+14, frame->getContentSize().height-78));
        thum->setAnchorPoint(Point(0, 0));
    }
    else if(post_user_type == "others" || post_user_type == "admin"){
        thum->setPosition(Point(-14, frame->getContentSize().height-78));
        thum->setAnchorPoint(Point(1, 0));
    }
    frame->addChild(thum,1,T_THUMB);
    
    
    ////投稿者名////
    Label* label_user_name = HMSFontUtil::createWithTTFBySystemFontArgument(user_name.c_str(), FONT_NAME_SYSTEM, 20);
    //右寄せ、左寄せ
    if(post_user_type=="mine") {
        label_user_name->setPosition(Point(frame->getContentSize().width-5, frame->getContentSize().height+3));
        label_user_name->setAnchorPoint(Point(1, 0));
    }
    else if(post_user_type == "others" || post_user_type == "admin"){
        label_user_name->setPosition(Point(5, frame->getContentSize().height+3));
        label_user_name->setAnchorPoint(Point(0, 0));
    }
    frame->addChild(label_user_name);
    
    
    ////投稿日時////
    string time = Utility::dbtimeFormat(insert_date, "%H:%M") ;
    Label* date = HMSFontUtil::createWithTTFBySystemFontArgument(time.c_str(), FONT_NAME_SYSTEM, 18);
    
    //右寄せ、左寄せ
    if(post_user_type=="mine") {
        date->setPosition(Point(-10,0));
		date->setHorizontalAlignment(TextHAlignment::RIGHT);
        date->setAnchorPoint(Point(1, 0));
    }
    else if(post_user_type == "others" || post_user_type == "admin"){
        date->setPosition(Point(frame->getContentSize().width+10, 0));
		date->setHorizontalAlignment(TextHAlignment::LEFT);
        date->setAnchorPoint(Point(0, 0));
    }
    frame->addChild(date) ;
    

    ////吹き出し////
    //枠色
    string balloon_file = "";
    if(post_user_type=="mine") balloon_file = "init/Team/chat/comment_head_myself.png";
    else if(post_user_type == "others")balloon_file = "init/Team/chat/comment_head_others.png";
    else if (post_user_type == "admin")balloon_file = "init/Team/chat/comment_head_admin.png";
    Sprite *balloon = Sprite::create(balloon_file.c_str());
    
    //右寄せ、左寄せ
    if(post_user_type=="mine") {
        balloon->setPosition(Point(frame->getContentSize().width-3, frame->getContentSize().height-10));
        balloon->setAnchorPoint(Point(0, 1));
    }
    else if(post_user_type == "others" || post_user_type == "admin"){
        balloon->setPosition(Point(4, frame->getContentSize().height-10));
        balloon->setAnchorPoint(Point(1, 1));
    }
    frame->addChild(balloon, Z_COMMENT_HEAD);
    
    return frame ;
}

void ChatListLayer::setup()
{
//    NormalScene *normal_scene = (NormalScene*)this->getParent()->getParent();
    
    ////リスト設定////
    string err;
    _json_chat_data = Json::parse(SaveData::loadString("team_chat_data_revision"), err);
    int json_count = countJsonData(_json_chat_data) ;
    
    //高さ合計
    int total_list_height = 0 ;
    total_list_height += 56; //送信ボタン分の高さ
    
    string check_change_date = "" ;
    if(json_count!=0) check_change_date = Utility::dbtimeFormat(_json_chat_data[toString<int>(json_count-1)]["insert_date"].string_value(), "%m月%d日") ; //リストの末尾から、日付が変わった時に表示を変更する
    
    string key_name = "team_chat_id_list_" + _chat_type;
    string team_chat_id_list = SaveData::loadString(key_name.c_str());
    HmsVector<string> id_list = split(team_chat_id_list, ":");
    
    for(int i=json_count-1; i>=0; i--) {
        string i_str = toString<int>(i);
        
        string chat_id_str = _json_chat_data[i_str]["chat_id"].string_value();
        int chat_id = fromString<int>(chat_id_str);
        string comment = _json_chat_data[i_str]["comment"].string_value();
        string user_name = _json_chat_data[i_str]["user_name"].string_value();
        string insert_date = _json_chat_data[i_str]["insert_date"].string_value();
        string user_master_id = _json_chat_data[i_str]["user_master_id"].string_value() ;
        string post_user_type = _json_chat_data[i_str]["post_user_type"].string_value() ;
        
        string image_path = "" ;
        if(post_user_type == "admin") {
            image_path =_json_chat_data[i_str]["image_path"].string_value();
        } else {
            string chara_id = _json_chat_data[i_str]["chara_id"].string_value();
            image_path =  "download/Chara/all100/" + chara_id + ".png" ;
        }
        
        bool found_flg = false;
        for(int j=0; j<id_list.size(); j++) {
            int id = fromString<int>(id_list.at(j));
            if(id==chat_id) found_flg = true;
        }
        
        //削除権限があるか
        bool delete_flg = false;
        if(SaveData::loadBool("team_leader_flg") || post_user_type=="mine") {
            delete_flg = true;
        }
      
        ////日付変更確認 日付が変わった時か、リストの一番上の時、時間を表示////
        string comment_date =  Utility::dbtimeFormat(_json_chat_data[i_str]["insert_date"].string_value(), "%m月%d日") ;
        if(comment_date != check_change_date){
            total_list_height += 80 ;
            
            Sprite *change_date_back = createChangeDate(check_change_date) ;
            change_date_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, total_list_height), 0.5, 0)) ;
            this->addChild(change_date_back, -1);
            total_list_height += 20 ;
        }
        
        ui::Scale9Sprite *frame = createMessageCell(comment, post_user_type, total_list_height, image_path, user_name, insert_date, found_flg) ;
        this->addChild(frame, -1);
        
        if(post_user_type=="others" && found_flg==false) {
            _frame_list.insert(frame);
            _chat_id_list.insert(chat_id);
        }
        _all_frame_list.insert(frame);
        _user_master_id_list.insert(user_master_id);
        
        total_list_height += frame->getContentSize().height + 70;
        
        //権限がある場合のみ削除ボタン表示
        if(delete_flg) {
            frame->setTag(i+1000);
        }
        
        
        //リストの一番上の時、時刻を表示する。
        if(i == 0){
            total_list_height += 80 ;
            
            Sprite *change_date_back = createChangeDate(check_change_date) ;
            change_date_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, total_list_height), 0.5, 0)) ;
            this->addChild(change_date_back, -1);
            total_list_height += 20 ;
        }
        
        
        //現在の吹き出しの時刻を保存
        check_change_date = comment_date ;
    }
    
    total_list_height += 120 ; //一番上の空白
    
    //スクロールの長さ
    int range = total_list_height ;
    if(range<0) range = 0;
    range *= ratio ;
    //this->setScrollRange(range);
    //一番下を初期表示
    _scroll_height = range;
    
    this->setTouchEnabled(true);
    
    this->schedule(schedule_selector(ChatListLayer::checkUnreadComment), 1.0f);
}

Sprite *ChatListLayer::createChangeDate(string change_date){
    Label* change_date_label = HMSFontUtil::createWithTTFBySystemFontArgument(change_date, FONT_NAME_SYSTEM, 20);
    
    Sprite *change_date_back = Sprite::create() ;
    change_date_back->setTextureRect(Rect(0, 0, change_date_label->getContentSize().width*1.5, change_date_label->getContentSize().height*1.5));
    change_date_back->setColor(Color3B::BLACK) ;
    change_date_back->setOpacity(150) ;
    change_date_back->setScale(ratio) ;
    
    change_date_label->setPosition(change_date_back->getContentSize()/2) ;
    change_date_back->addChild(change_date_label);

    return change_date_back ;
}

void ChatListLayer::checkUnreadComment(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
     
    for(int i=0; i<_frame_list.size(); i++) {
        //HMSLOG("check %d", i);
        ui::Scale9Sprite *frame = _frame_list.at(i);
        int chat_id = _chat_id_list.at(i);
        Sprite *red = (Sprite *)frame->getChildByTag(T_RED);
        if(red==nullptr) continue;
        Point local_point = red->getPosition();
        Point world_point = red->convertToWorldSpace(local_point);
        //HMSLOG("%f,%f", world_point.x, world_point.y);
        //画面内に入ってきたら消す
        if(world_point.y<visibleSize.height-10 && world_point.y>10) {
            DelayTime *delay = DelayTime::create(0.4f);
            FadeOut *fade_out = FadeOut::create(0.6f);
            RemoveSelf *remove = RemoveSelf::create();
            ChatBaseUpperLayer* upperLayer = (ChatBaseUpperLayer*)this->getParent()->getParent();
            if(upperLayer->_chat_type == "team"){
                int count = 0;
                for(int j=0; j<_frame_list.size(); j++) {
                    ui::Scale9Sprite *countChechFrame = _frame_list.at(j);
                    Sprite *red = (Sprite *)countChechFrame->getChildByTag(T_RED);
                    if(red==nullptr) continue;
                    count++;
                }
                _unread_count = count - 1;
                CallFunc *countDown = CallFunc::create([this, upperLayer](){
                
                    if(_unread_count <= 0) upperLayer->dispawnNewMark();
                    upperLayer->_mark_number_label->setString(toString<int>(_unread_count));
                });
                red->runAction(Sequence::create(delay, fade_out, remove, countDown, NULL));
            }else{
                red->runAction(Sequence::create(delay, fade_out, remove, NULL));
            }
            string key_name = "team_chat_id_list_" + _chat_type;
            string team_chat_id_list = SaveData::loadString(key_name.c_str());
            if(team_chat_id_list=="") team_chat_id_list = toString(chat_id);
            else {
                HmsVector<string> id_list = split(team_chat_id_list, ":");
                bool found_flg = false;
                for(int i=0; i<id_list.size(); i++) {
                    int id = fromString<int>(id_list.at(i));
                    if(id==chat_id) found_flg = true;
                }
                if(found_flg==false) {
                    team_chat_id_list += ":" + toString(chat_id);
                }
            }
            SaveData::saveString(key_name.c_str(), team_chat_id_list);
            //SaveData::saveString(key_name.c_str(), "");
        }
    }
}


float ChatListLayer::getScrollHeight()
{
    return _scroll_height ;
}

bool ChatListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    for(vector<ui::Scale9Sprite *>::iterator iterator = _all_frame_list.begin(); iterator != _all_frame_list.end(); iterator++){
        ui::Scale9Sprite *frame = *iterator ;
        if(frame){
            Sprite *thum = (Sprite *)frame->getChildByTag(T_THUMB);
            if(Utility::containsPoint(thum, touch->getLocation())){
                int index = (int)(iterator-_all_frame_list.begin()) ;
                
                string user_master_id = _user_master_id_list[index] ;
                
                if(user_master_id != SaveData::loadString("user_master_id") && user_master_id!="0" && user_master_id != ""){
                    HMSLOG("user_master_id: %s" ,user_master_id.c_str());
                    HMSLOG("user_data_list: %s" ,_json_user_data_list[user_master_id].dump().c_str());
                    
                    string text = "";
                    
                    UserDetailContainFriendIdSprite *sprite = UserDetailContainFriendIdSprite::create();
                    sprite->setScale(0) ;
                    sprite->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
                    sprite->setupUserData(_json_user_data_list[user_master_id]) ;
                    sprite->showUserData() ;
                    
                    ChatBaseUpperLayer *chat_base_upper_layer = (ChatBaseUpperLayer *)this->getParent()->getParent() ;
                    if(chat_base_upper_layer->checkPositionContainChatPlace(touch->getLocation())){
                        chat_base_upper_layer->showPopupSprite(sprite) ;
                    }
                }
            }
        }
    }
    
    /*
    ////タップ判定////
    for(int i=1000; i<chat_list.size()+1000; i++) {
        Sprite *sprite = (Sprite*)this->getChildByTag(i);
        if(sprite==NULL) continue;
        
        if(Utility::containsPoint(sprite, touch->getLocation())) {
            ////確認////
            string url = HOST;
            url += "team/team_comment_delete.php";
            
            url += "?user_master_id=";
            string user_master_id = SaveData::loadString("user_master_id");
            url += user_master_id;
            
            url += "&team_id=";
            url += toString<int>(SaveData::loadInt("team_id"));
            
            url += "&team_comment_id=";
            url += this->chat_list.getValue(i-1000, "chat_id");
            
            //HMSLOG("url=%s",  url.c_str());
            
            ChatListBaseLayer *base = (ChatListBaseLayer*)this->getParent()->getParent();
            //SEL_CallFuncND selector = callfuncND_selector(ChatListBaseLayer::httpTeamCommentDeleteCompleted);
            string text = "削除してもよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this, base, url](){
                if(base){
                    auto selector = httpresponse_selector(ChatListBaseLayer::httpTeamCommentDeleteCompleted);
                    HttpConnect::sendHttpRequest(base, url, selector);
                }
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            base->addChild(confirm, 9999);
            
        }
    }
     */
    return true;
}

void ChatListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void ChatListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void ChatListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

Size ChatListLayer::calcDimensions(string comment_text, float font_size)
{
    float dimensions_x = 0;
    float dimensions_y = 0 ;

    int line_count = 0 ;
    HmsVector<string> vec_text = split(comment_text.c_str(), "\n");
    int max_line_length = 0;
    for (int i = 0; i < vec_text.size(); i++){
        line_count++ ;
        
        float length = calcStrLength(vec_text[i]) ;
        line_count += length / 15 ;
        if(length > max_line_length) max_line_length = length ;
    }

    if(max_line_length >= 15){
        dimensions_x = font_size * 15 ;
    }else{
        dimensions_x = font_size * max_line_length ;
    }

    dimensions_y = line_count * (font_size+4) ; //y軸方向には、余白4+フォントサイズ分の高さを確保する。
    
    return Size(dimensions_x, dimensions_y) ;
}

/*
void ChatListLayer::xtTouchesBegan(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
}
void ChatListLayer::xtTouchesMoved(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
}
void ChatListLayer::xtTouchesEnded(cocos2d::CCSet* _touches, cocos2d::CCEvent* event)
{
    //タップ不可能なら処理を行わない
    //if(scroll_tap_flg==false) return;
    
    CCSetIterator it;
    }
}
*/

void ChatListLayer::createScrollView()
{
//    auto pScrollView = ScrollView::create();
//    pScrollView->setAnchorPoint(Point::ZERO);
//    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
//    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio)- (FOOTER_SIZE * ratio)));
//    
//    //縦方向にスクロール設定
//    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
//    
//    //スクロールのバウンスを止める
//    pScrollView->setBounceable(true);
//    pScrollView->setDelegate(this);
//    this->addChild(pScrollView, 1, SCROLLVIEW_TAG);
//    
//    //PresentMenuのレイヤーを戻り値に指定して使う
//    auto layer = createList();
//    layer->setTag(SCROLLLAYER_TAG);
//    pScrollView->setContainer(layer);
//    
//    //スクロールさせる範囲の設定
//    pScrollView->setContentSize(Size(_contentSize.width, cell_size * _banner_sprite.size()));
//    
//    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
//    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _banner_sprite.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
//    
}

int ChatListLayer::getUnreadCount(){
    return this->_unread_count;
}