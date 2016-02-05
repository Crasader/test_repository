#include "BattleFreeConfirmLayer.h"
#include "PlistAnimationSprite.h"

bool BattleFreeConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleFreeConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ページ名の設定
    setName("BattleFree");
    
    return true;
}

void BattleFreeConfirmLayer::setup()
{
	//レイヤーの後ろにタップ処理が伝搬しないように設定する
	listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BattleFreeConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleFreeConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleFreeConfirmLayer::onTouchEnded, this);
	//タップイベントの登録
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	//背景となる影レイヤー
	auto shadow_layer = LayerColor::create(Color4B(0, 0, 0, 100));
	this->addChild(shadow_layer);
	
    string tips_list_dump = SaveData::loadString("tips_list");
    string err;
    _tips_json = Json::parse(tips_list_dump, err);
    
	//テキスト「対戦相手をさがしています」
	Label *message_label = HMSFontUtil::createWithTTFBySystemFontArgument("対戦相手を探しています...", FONT_NAME_SYSTEM, 32);
    message_label->setScale(ratio) ;
	message_label->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 300), 0.5, 0.5));
	this->addChild(message_label, 1, T_MESSAGE);
	
    //キャンセルボタンの作成
	cancel_btn = MenuItemImage::create(IMG_CANCEL_BT, IMG_CANCEL_BT, CC_CALLBACK_0(BattleFreeConfirmLayer::cancelWaitBattle, this));
    if(countJsonData(_tips_json)>0){
    	cancel_btn->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - 310), 0.5, 0.5));
    }else{
    	cancel_btn->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5)); //tipsが無い場合はキャンセルボタンを中心に表示する
    }
    cancel_btn->setScale(ratio) ;
	Menu* menu = Menu::create(cancel_btn, nullptr);
    menu->setPosition(Point::ZERO);
	this->addChild(menu, 1, 5);
	
	//ローディングの表示
	//showLoadingEffect(this, "init/Loading/Sprite/loading.plist", convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 190), 0.5, 0.5), ratio * 7/10); //NowLoadingを7/10スケールにする
    auto loading_sprite = PlistAnimationSprite::setData(this, "init/Loading/Sprite/loading", 0, 0.06f, 10000);
    loading_sprite->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 190), 0.5, 0.5));
    loading_sprite->setScale(ratio * 7/10);//NowLoadingを7/10スケールにする
    loading_sprite->setTag(T_ANIMATION) ;
    
    
    
    cancel_flg = false;//キャンセルフラグの初期化
	waitBattle();
    
    //tipsを作成
    createTips() ;
    
    //tipsを表示
    showTipsAction() ;
}

void BattleFreeConfirmLayer::showTipsAction(){
    int json_count = countJsonData(_tips_json);
    if(json_count>0){
        
        auto start_func = CallFunc::create([this, json_count](){
            //nextの矢印と文字を見えなくする
            _next->setVisible(false);
            _next_shape->setVisible(false);
        });
        
        //詳細をひっこめるアクション
        MoveTo *move_hide = MoveTo::create(0.1, convertFromDisplaySize(Point(0, DISPLAY_HEIGHT/2 + 70), 0.5, 0.5)) ;
        FadeTo *fadeTo_hide = FadeTo::create(0.1, 0) ;
        Spawn *spawn_hide = Spawn::createWithTwoActions(move_hide,fadeTo_hide) ;
        
        //詳細の内容を変更する関数
        auto content_change_func = CallFunc::create([this, json_count](){
            
                //tipsの表示内容をランダムに決定
                string tips_index = toString<int>(random(0, json_count-1)) ;

                //タイトル
                string tips_title_str = _tips_json[tips_index]["title"].string_value();
            	_tips_title->setString(tips_title_str);
                
                //内容
                string tips_comment_str = _tips_json[tips_index]["comment"].string_value();
                tips_comment_str = Utility::replaceStr(tips_comment_str, "n", "\n"); //nを改行文字に置き換える
                _tips_comment->setString(tips_comment_str);
                
                //左右のキャラの画像を1~14の値でランダムに決定
                int left_chara_no = random(1, 14) ;
                int right_chara_no ;
                while(true){ //left_chara_noとright_cahra_noが重複しない値になるまでループ
                    right_chara_no = random(1, 14) ;
                    if(left_chara_no!=right_chara_no)break;
                }

                _left_chara->setTexture("init/Home/Quest/Online/tips_chara" + toString<int>(left_chara_no) + ".png");
                _right_chara->setTexture("init/Home/Quest/Online/tips_chara" + toString<int>(right_chara_no) + ".png");
        });
        
        //位置を変えるアクション
        MoveTo *move_start_place = MoveTo::create(0, convertFromDisplaySize(Point(DISPLAY_HEIGHT, DISPLAY_HEIGHT/2 + 70), 0.5, 0.5)) ;
        
        //出現するアクション
        MoveTo *move_appear = MoveTo::create(0.2, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 70), 0.5, 0.5)) ;
        EaseExponentialOut *ease_out_appear = EaseExponentialOut::create(move_appear);
        FadeTo *fadeTo_appear = FadeTo::create(0.1, 255) ;
        Spawn *spawn_appear = Spawn::createWithTwoActions(ease_out_appear, fadeTo_appear) ;
        
        
        auto end_func = CallFunc::create([this, json_count](){
            //nextの矢印と文字を見えるようにする
            _next->setVisible(true);
            _next_shape->setVisible(true);
        });
        
        
        _tips_frame->runAction(Sequence::create(start_func, spawn_hide, content_change_func, move_start_place, spawn_appear, end_func, nullptr)) ;
    }
}

void BattleFreeConfirmLayer::createTips()
{
    int json_count = countJsonData(_tips_json);
    
    if(json_count > 0){
        
    	//tipsのフレーム構築
    	_tips_frame = Sprite::create("init/Home/Quest/Online/tips_bk1.png");
    	_tips_frame->setScale(ratio);
    	_tips_frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 70), 0.5, 0.5));
    	_tips_frame->setAnchorPoint(Point(0.5, 1));
    	this->addChild(_tips_frame);
        
    	//tipsのメッセージ表示部を作成 (キャラの上に覆い被さるように前後関係を設定するためにtips_bk1.pngとパーツが異なる)
    	Sprite *message_area = Sprite::create("init/Home/Quest/Online/tips_bk2.png");
        message_area->setAnchorPoint(Point(0,0)) ;
    	_tips_frame->addChild(message_area,3);
        
    	_tips_title = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 34);
    	_tips_title->setPosition(Point(_tips_frame->getContentSize().width/2, 250));
    	_tips_frame->addChild(_tips_title, 4);
        
    	_tips_comment = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 28);
        _tips_comment->setAnchorPoint(Point(0.5,1)) ;
        _tips_comment->setVerticalAlignment(TextVAlignment::TOP);
        _tips_comment->setDimensions(520, 160);
    	_tips_comment->setPosition(Point(_tips_frame->getContentSize().width/2, 210));
    	_tips_frame->addChild(_tips_comment, 4);
        
        //左のキャラを作成
        _left_chara = Sprite::create();
        _left_chara->setAnchorPoint(Point(0.5, 0));
        _left_chara->setPosition(Point(133, 285));
        _tips_frame->addChild(_left_chara,2) ;
        
        //右のキャラを作成
        _right_chara = Sprite::create();
        _right_chara->setAnchorPoint(Point(0.5, 0));
        _right_chara->setPosition(Point(487, 285));
        _right_chara->setFlippedX(true) ; //Y軸
        _tips_frame->addChild(_right_chara,2) ;
        
        //next文字
        _next = HMSFontUtil::createWithTTFBySystemFontArgument("NEXT", FONT_NAME_SYSTEM, 30);
        _next->setHorizontalAlignment(TextHAlignment::RIGHT);
        _next->setVerticalAlignment(TextVAlignment::CENTER);
        _next->setAnchorPoint(Point(1, 1));
        _next->setPosition(Point(_tips_frame->getContentSize().width - 50, 0));
        _tips_frame->addChild(_next) ;
        
        //三角形
        _next_shape = getTriangleShape(Point(0,1)) ;
        _next_shape->setPosition(Point(_tips_frame->getContentSize().width - 40, 0));
        _tips_frame->addChild(_next_shape) ;
        
    }
}

DrawNode* BattleFreeConfirmLayer::getTriangleShape(const Point& anchorPoint)
{
    float triangle_width = 20;
    float triangle_height = 30;
    
    //下半分のテスト
    auto shape = DrawNode::create();
    static Vec2 triangle[3];

    triangle[0] = Vec2(0 - anchorPoint.x*triangle_width, 0 - anchorPoint.y*triangle_height);
    triangle[1] = Vec2(0 - anchorPoint.x*triangle_width, triangle_height - anchorPoint.y*triangle_height);
    triangle[2] = Vec2(triangle_width - anchorPoint.x*triangle_width, triangle_height/2 - anchorPoint.y*triangle_height);
    
    static Color4F green(0, 1, 0, 1);
    shape->drawPolygon(triangle, 3, Color4F::WHITE, 1, Color4F::GRAY);
    return shape;
}

void BattleFreeConfirmLayer::waitBattle()
{
    if(cancel_flg == false){//キャンセルしてない時
        string user_master_id = SaveData::loadString("user_master_id");
        string url = HOST;
        url += "battle/regist_arena.php?user_master_id=" + user_master_id;
        
        url += "&deck_no=" + SaveData::loadString("party_no");//デッキナンバー追加
        
        //auto http_connect = HttpConnect::create();
        auto selector = httpresponse_selector(BattleFreeConfirmLayer::waitBattleCallback);
        HttpConnect::sendHttpRequest(this, url, selector);
    }
}

void BattleFreeConfirmLayer::waitBattleCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("response:%s", str.c_str());
    //通信エラー以外
    if(str != "not connect" && str != "response NG") {
        if(str.find("battle:", 0)!=string::npos)
        {
            HmsVector<string> str_list = split(str, ":");
            int battle_list_id = fromString<int>(str_list.at(1));
            string enemy_user_master_id = str_list.at(3);
            
            //対戦相手が来たらDungeonSceneへ移行
            Utility::setOnlineBattleData(battle_list_id, enemy_user_master_id);
            
            //オンラインバトルタイプ（初心の手引きのフラグで使用）
            SaveData::saveString("online_battle_type", "quick");
            
            auto scene = DungeonScene::createScene();
            auto director = Director::getInstance();
            director->replaceScene(scene);
        } else if(str=="wait") {//待機中
            HMSLOG("waiting...");

            DelayTime *delay = DelayTime::create(1.0f);
            CallFunc *expire1 = CallFunc::create([this]()
                                                 {
                                                     this->waitBattle();
                                                 });//ラムダ式
            this->runAction(Sequence::create(delay, expire1, NULL));
        } else {
            
        }
    }
}

void BattleFreeConfirmLayer::cancelWaitBattle()
{
    cancel_btn->setEnabled(false);
    cancel_flg = true;
    
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "battle/cancel_arena.php?user_master_id=" + user_master_id;
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        cancelWaitBattleCallback(sender, response);
    });
}

void BattleFreeConfirmLayer::cancelWaitBattleCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("response:%s", str.c_str());
    //通信エラー以外
    if(str != "not connect" && str != "response NG") {
		//現在表示している物を消去
        Node *anim = this->getChildByTag(T_ANIMATION) ;
        Node *label = this->getChildByTag(T_MESSAGE) ;
        if(anim) anim->removeFromParent() ;
        if(label) label->removeFromParent() ;
        
        //メッセージ表示
        Label *message_label = HMSFontUtil::createWithTTFBySystemFontArgument("エントリーをキャンセルしました！", FONT_NAME_SYSTEM, 30);
        message_label->setScale(ratio);
        message_label->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 180), 0.5, 0.5));
        this->addChild(message_label, 0, 3);
		
		this->runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([&](){
			static_cast<NormalScene* >(getParent()->getParent())->changePage("BattleTop");
			this->removeFromParentAndCleanup(true);
		}), nullptr));
    }

}

bool BattleFreeConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch BattleFreeConfirmLayer!");
    showTipsAction() ;
    
    return true;
}
void BattleFreeConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BattleFreeConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}