#include "CharaDetailSpriteList.h"
#include "CharaDetailSprite.h"

CharaDetailSpriteList* CharaDetailSpriteList::create(HmsVector<int> chara_list_id_list, int chara_list_id, bool hide_lock_button){

    CharaDetailSpriteList *sprite = new CharaDetailSpriteList();

    if (sprite && sprite->init(chara_list_id_list, chara_list_id, hide_lock_button))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaDetailSpriteList::init(HmsVector<int> chara_list_id_list, int chara_list_id, bool hide_lock_button)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    _chara_list_id_list = chara_list_id_list;
    _chara_list_id = chara_list_id;
    _hide_lock_button = hide_lock_button;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    scroll_width = visibleSize.width;
    //HMSLOG("scroll_width=%f", scroll_width);
    base_y = convertFromDisplaySize(Point(0, DISPLAY_HEIGHT/2), 0.5, 0.5).y;
    
    listener = EventListenerTouchOneByOne::create();
    
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの設定
    listener->onTouchBegan = CC_CALLBACK_2(CharaDetailSpriteList::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaDetailSpriteList::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaDetailSpriteList::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    int initial_index = 0;
    for(int i=0; i<_chara_list_id_list.size(); i++) {
        int id = _chara_list_id_list.at(i);
        if(id==_chara_list_id) initial_index = i;
    }
    base_index = initial_index;
    
    //json作成
    string json_text = SaveData::loadString("user_chara_list");
    string json_text_as = SaveData::loadString("user_active_skill_list");
    string json_text_ps = SaveData::loadString("user_passive_skill_list");
    string err2;
    string err3;
    string err4;
    json11::Json json_chara = json11::Json::parse(json_text, err2);
    json11::Json json_active_skill = json11::Json::parse(json_text_as, err3);
    json11::Json json_passive_skill = json11::Json::parse(json_text_ps, err4);
    
    //スキル配列作成
    map<string, string> map_chara;
    map<string, string> map_active_skill;
    map<string, string> map_passive_skill;
    int max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));
    string index_str, key, value;
    for(int i=0; i<max_chara_count; i++) {
        index_str = toString(i);
        key = json_chara[index_str]["chara_list_id"].string_value();
        value = json_chara[index_str]["active_skill_id"].string_value() + "_" + json_chara[index_str]["passive_skill_id"].string_value();
        map_chara.insert(pair<string, string>(key, value));
    }
    for(int i=0; i<max_chara_count; i++) {
        index_str = toString(i);
        key = json_active_skill[index_str]["active_skill_id"].string_value();
        value = json_active_skill[index_str]["name"].string_value() + "_" + json_active_skill[index_str]["detail"].string_value() + "_"
        + json_active_skill[index_str]["icon_detail"].string_value() + "_" + json_active_skill[index_str]["need_cost"].string_value();
        map_active_skill.insert(pair<string, string>(key, value));
    }
    for(int i=0; i<max_chara_count; i++) {
        index_str = toString(i);
        key = json_passive_skill[index_str]["passive_skill_id"].string_value();
        value = json_passive_skill[index_str]["name"].string_value() + "_" + json_passive_skill[index_str]["detail"].string_value() + "_"
        + json_passive_skill[index_str]["icon_detail"].string_value();
        map_passive_skill.insert(pair<string, string>(key, value));
    }
    
    //キャラ一覧
    float ratio = getVisibleRatio();
    CharaDetailSprite* _chara_detail_sprite;
    for(int i=0; i<_chara_list_id_list.size(); i++) {
        int id = _chara_list_id_list.at(i);
        
        //座標と拡大比率
        int x = (i - initial_index + 0.5) * scroll_width;
        if(i==0) max_x = -x + scroll_width;
        if(i==_chara_list_id_list.size()-1) min_x = -x;
//        if(i==_chara_list_id_list.size()-1) min_x = -x-scroll_width;
        
        float r = ratio;
        if(i==initial_index) r = 0;
        
        //左右のアイコン有無
        bool left_flg = true;
        bool right_flg = true;
        float delay_time = 0.0f;
        if(i==0) left_flg = false;
        if(i==_chara_list_id_list.size()-1) right_flg = false;
        delay_time = abs(i-initial_index)*0.5f;
        
        //スキル名などの決定
        string skill_name = "";
        string skill_detail = "";
        string skill_icon_detail = "";
        string need_cost = "";
        string value;
        bool switch_skill_button_flg = false;
        map<string, string>::iterator itr;
        itr = map_chara.find(toString(id));
        if(itr!=map_chara.end()) {
            value = itr->second;
            HmsVector<string> vec = split(value, "_");
            string active_skill_id = vec.at(0);
            string passive_skill_id = vec.at(1);
            
            //HMSLOG("active_skill_id=%s, passive_skill_id=%s", active_skill_id.c_str(), passive_skill_id.c_str());
            if(active_skill_id!="0") {
                if(passive_skill_id!="0") switch_skill_button_flg = true;
                
                itr = map_active_skill.find(active_skill_id);
                if(itr!=map_active_skill.end()) {
                    value = itr->second;
                    HmsVector<string> vec = split(value, "_");
                    skill_name = vec.at(0);
                    skill_detail = vec.at(1);
                    skill_icon_detail = vec.at(2);
                    need_cost = vec.at(3);
                }
            } else {
                itr = map_passive_skill.find(passive_skill_id);
                if(itr!=map_passive_skill.end()) {
                    value = itr->second;
                    HmsVector<string> vec = split(value, "_");
                    skill_name = vec.at(0);
                    skill_detail = vec.at(1);
                    skill_icon_detail = vec.at(2);
                }
            }
        }
        HMSLOG("skill_name=%s, need_cost=%s", skill_name.c_str(), need_cost.c_str());
        
        //キャラ画像
        _chara_detail_sprite = CharaDetailSprite::create(id, _hide_lock_button);
        _chara_detail_sprite->setSkillInfo(json_chara, skill_name, skill_detail, skill_icon_detail, need_cost);
        _chara_detail_sprite->setSwitchSkillButtonFlg(switch_skill_button_flg);
        _chara_detail_sprite->setLeftRightFlg(left_flg, right_flg, delay_time);
        _chara_detail_sprite->setScale(r);
        _chara_detail_sprite->setPosition(Point(x, base_y));
        //this->addChild(_chara_detail_sprite,500);

    	//背景となる影レイヤー
    	auto shadow = LayerColor::create(Color4B(0, 0, 0, 0));
        shadow->setPosition(Point(x-scroll_width/2, 0));
        shadow->runAction(FadeTo::create(0.2f, 100)) ;
    	this->addChild(shadow,499);
        
//        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
//        shadow->setScale(ratio);
//        shadow->setOpacity(0);
//        shadow->setPosition(Point(x, base_y));
//        shadow->runAction(FadeIn::create(0.2f)) ;
//        this->addChild(shadow, 499);
        
        //SE
        if(i==initial_index) {
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
            auto scale = ScaleTo::create(0.2f, 1.0*ratio);
            _chara_detail_sprite->runAction(scale);
        }
        this->addChild(_chara_detail_sprite,500);
        
        //ウィンドウが閉じられたときに呼び出されるコールバックメソッドを登録
        SEL_SCHEDULE s = schedule_selector(CharaDetailSpriteList::notifyClose);
        _chara_detail_sprite->setSchedulerForClose(this, s);

    }
    
    //左端のカード詳細より左側の陰を作成
    int x_left_over_shadow = (-1 - initial_index + 0.5) * scroll_width; //マイナス1は
	auto shadow_left_over = LayerColor::create(Color4B(0, 0, 0, 0));
    shadow_left_over->setPosition(Point(x_left_over_shadow-scroll_width/2, 0));
    shadow_left_over->runAction(FadeTo::create(0.2f, 100)) ;
	this->addChild(shadow_left_over,499);
    
    //右端のカード詳細より右側の陰を作成
    int x_right_over_shadow = (20 - initial_index + 0.5) * scroll_width;
	auto shadow_right_over = LayerColor::create(Color4B(0, 0, 0, 0));
    shadow_right_over->setPosition(Point(x_right_over_shadow-scroll_width/2, 0));
    shadow_right_over->runAction(FadeTo::create(0.2f, 100)) ;
	this->addChild(shadow_right_over,499);
    
//    //左端のカード詳細より左側の陰を作成
//    int x_left_over_shadow = (-1 - initial_index + 0.5) * scroll_width; //マイナス1は
//    auto shadow_left_over = Sprite::create("init/Home/Common/back_shadow50.png");
//    shadow_left_over->setScale(ratio);
//    shadow_left_over->setOpacity(0);
//    shadow_left_over->setPosition(Point(x_left_over_shadow, base_y));
//    shadow_left_over->runAction(FadeIn::create(0.2f)) ;
//    this->addChild(shadow_left_over, 499);
//    
//    //右端のカード詳細より右側の陰を作成
//    int x_right_over_shadow = (20 - initial_index + 0.5) * scroll_width;
//    auto shadow_right_over = Sprite::create("init/Home/Common/back_shadow50.png");
//    shadow_right_over->setScale(ratio);
//    shadow_right_over->setOpacity(0);
//    shadow_right_over->setPosition(Point(x_right_over_shadow, base_y));
//    shadow_right_over->runAction(FadeIn::create(0.2f)) ;
//    this->addChild(shadow_right_over, 499);
    
    return true;
}

//閉じたときの通知を行う
void CharaDetailSpriteList::notifyClose(float dt)
{
    if(_parent!=nullptr && _sel_for_close!=nullptr) {
        _parent->scheduleOnce(_sel_for_close, 0.0f);
        _sel_for_close = nullptr;
    }
}

//閉じたときの通知を行う
void CharaDetailSpriteList::notifyChangeBaseIndex(float dt)
{
    if(_parent!=nullptr && _sel_for_change!=nullptr) {
        _parent->scheduleOnce(_sel_for_change, 0.0f);
    }
}

//窓を閉じるときに呼び出すメソッドを登録する
void CharaDetailSpriteList::setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel)
{
    _parent = parent;
    _sel_for_close = sel;
}

void CharaDetailSpriteList::setSchedulerForChangeBaseIndex(Layer *parent, SEL_SCHEDULE sel) {
    _parent = parent;
    _sel_for_change = sel ;
}

bool CharaDetailSpriteList::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(move_flg==true) return true;
    
    swipe_ratio = 1.0f;
    
    _touchPoint = touch->getLocation();
    return true;
}

void CharaDetailSpriteList::onTouchMoved(Touch *touch, Event *unused_event)
{
    if(move_flg==true) return;
    
    float diff_x = touch->getLocation().x - _touchPoint.x;
    diff_x *= swipe_ratio;
    if(diff_x<0) swipe_ratio -= diff_x / 100.0f;
    else swipe_ratio += diff_x / 100.0f;
    if(swipe_ratio>3) swipe_ratio = 1.5f;
    HMSLOG("swipe_ratio=%f", swipe_ratio);
    Point now_position = this->getPosition();
    float new_x = now_position.x + diff_x;
    //HMSLOG("min_x=%f,max_x=%f,new_x=%f", min_x, max_x, new_x);

    if(new_x < min_x) new_x = min_x;
    if(new_x > max_x) new_x = max_x;
    this->setPosition(Point(new_x, now_position.y));
    _touchPoint = touch->getLocation();
}

void CharaDetailSpriteList::onTouchEnded(Touch *touch, Event *unused_event)
{
    if(move_flg==true) return;
    
    swipe_ratio = 1.0f;
    
    Point now_position = this->getPosition();
    HMSLOG("x=%f", now_position.x);
    
    int initial_index = 0;
    for(int i=0; i<_chara_list_id_list.size(); i++) {
        int id = _chara_list_id_list.at(i);
        if(id==_chara_list_id) initial_index = i;
    }
    
    for(int i=0; i<_chara_list_id_list.size(); i++) {
        int x = (initial_index - i) * scroll_width;
        int upper_x = x+scroll_width/2;
        int lower_x = x-scroll_width/2;
        if(now_position.x >= lower_x && now_position.x <= upper_x) {
            HMSLOG("move to index=%d, x=%d", i, x);
            move_flg = true;
            MoveTo *move = MoveTo::create(0.1f, Point(x, 0));
            
            CallFunc *expire1 = CallFunc::create([this]()
                                                 {
                                                     move_flg = false;
                                                 });//ラムダ式
            this->runAction(Sequence::create(move, expire1, NULL));

            //SE
            if(i!=base_index) {
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/card_slash.mp3",false);
            } else {
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3", false);
            }
            base_index = i;
            
            notifyChangeBaseIndex(0) ;
            return;
        }
    }
}



int CharaDetailSpriteList::getBaseIndex(){
    return base_index;
}