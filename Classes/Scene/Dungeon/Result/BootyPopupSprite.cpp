#include "BootyPopupSprite.h"
#include "ResultLayer.h"

BootyPopupSprite::BootyPopupSprite()
:_now_container_index(0)
,_max_container_index(0)
,_now_swipe(false)
{
}

BootyPopupSprite* BootyPopupSprite::create(const string filename, Json json)
{
    BootyPopupSprite *pRet = new BootyPopupSprite();
    
    if (pRet && pRet->init(filename, json)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool BootyPopupSprite::init(string filename, Json json)
{
    if(!Sprite::initWithFile(filename))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期データの読み込み
    loadData(json);
    
    //アイテムの表示を4つとしたときの、表示数の最後のindex
    _max_container_index = _all_item_count / 4 ;
    if(_all_item_count % 4 == 0){
        _max_container_index = _max_container_index - 1 ;
    }
    
    return true;
}

void BootyPopupSprite::loadData(Json json)
{
    //初期データの読み込み
	_reward_item_list = json["item_list_data"].object_items();
	_all_item_count = countJsonData(_reward_item_list);
}

void BootyPopupSprite::createItemList()
{
    //アイテムリストの作成
	for (int i = 0; i < _all_item_count; i++)
	{
		_item_list.push_back(_reward_item_list[toString<int>(i)]);
	}
}

void BootyPopupSprite::createItemSprite()
{
    for (int i = 0; i < _all_item_count; i++) {
//        if (i == 4) break;
        string img_pass = getSpritePass(_item_list[i]["item_type"].string_value(), _item_list[i]["related_primary_id"].string_value());
        HMSLOG("img_pass = %s", img_pass.c_str());
        _item_sprite.push_back(Sprite::create(img_pass));
        _clipper->addChild(_item_sprite[i], 1, TAG_REWARD_ITEM);
    }
}

void BootyPopupSprite::createClippingNode()
{
    //獲得戦利品のポップアップ "init/Dungeon/result/popup_booty.png" の内側の赤い部分をかたどるRectを生成
    Rect rect = Rect(0, 0, 530, 350);
    Sprite* mask = Sprite::create();
    mask->setTextureRect(rect);
    mask->setPosition(this->getContentSize()/2) ;
    /*
    Sprite *popupSprite = Sprite::create("init/Dungeon/result/popup_booty.png") ;
    popupSprite->setAnchorPoint(Point(0,0));
     */
    
	_clipper = ClippingNode::create();
	_clipper->setStencil(mask);
	_clipper->setAlphaThreshold(0.01f);
	this->addChild(_clipper);
}

void BootyPopupSprite::showItemSprite(int container_index)
{
    int offset = container_index * 4;
    
    int item_count = _all_item_count - offset ;
    if(item_count > 4)item_count = 4;
    
    if (item_count == 1) {
        _item_sprite[offset]->setPosition(this->getContentSize().width / 2 - 50 + container_index * this->getContentSize().width, this->getContentSize().height / 2);
    }
    else if (item_count == 2) {
        for (int i = 0; i < item_count; i++)
        {
            _item_sprite[offset+i]->setPosition(Point(ITEM_1_POS_X + (ITEM_DISTANCE_X * (i % 2)) + container_index * this->getContentSize().width, this->getContentSize().height / 2));
        }
    }
    else
    {
        for (int i = 0; i < item_count; i++)
        {
//            if (i == 4) break;
            _item_sprite[offset+i]->setPosition(Point(ITEM_1_POS_X + (ITEM_DISTANCE_X * (i % 2)) + container_index * this->getContentSize().width, ITEM_1_POS_Y - (ITEM_DISTANCE_Y * (i / 2))));
        }
    }
}

string BootyPopupSprite::getSpritePass(string item_type, string related_primary_id)
{
    //スプライトのパスを取得
    string pass = "download/Chara/parts100/card_bk1_reverse.png";
    
    if (item_type == "jewel")
    {
        pass = "init/Present/item.png";
    }
    else if (item_type == "gold")
    {
        int gold_id = std::stoi(related_primary_id);
        if (gold_id > 0) {
            pass = "init/Present/item" + related_primary_id + ".png";
        } else {
            pass = "init/Present/item2.png";
        }
    }
    else if (item_type == "chara")
    {
        pass = "download/Chara/thumb100/" + related_primary_id + ".png";
    }
    
    return pass;
}

void BootyPopupSprite::showItemName(int container_index)
{
    int offset = container_index * 4;
    
    int item_count = _all_item_count - offset ;
    if(item_count > 4)item_count = 4;
    
    //獲得品アイテムの名前の表示
    for (int i = 0; i < item_count; i++)
    {
//        if (i == 4) break;
        
        string item_name = "";
        if (_item_list[offset+i]["item_type"].string_value() == "jewel")
        {
            item_name = "宝玉";
        }
        else if (_item_list[offset+i]["item_type"].string_value() == "gold")
        {
            item_name = "資金";
        }
        else if (_item_list[offset+i]["item_type"].string_value() == "chara")
        {
            item_name = _item_list[offset+i]["chara_name"].string_value();
        }
        
        item_name += "\n×" + _item_list[offset+i]["count"].string_value();
        
        auto item_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(item_name, FONT_NAME_SYSTEM, 28);
        item_name_label->setAnchorPoint(Point(0, 0.5));
        item_name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        item_name_label->setPosition(Point(_item_sprite[offset+i]->getPosition().x + _item_sprite[offset+i]->getContentSize().width / 2 + 10, _item_sprite[offset+i]->getPosition().y));
        item_name_label->enableOutline(Color4B(0, 0, 0, 255), 1);
        _clipper->addChild(item_name_label, 2, TAG_REWARD_ITEM);
    }
}

void BootyPopupSprite::showRewardDescription(int container_index){
    int offset = container_index * 4;
    
    int item_count = _all_item_count - offset ;
    if(item_count > 4)item_count = 4;
    
    //獲得品アイテムの名前の表示
    for (int i = 0; i < item_count; i++)
    {
//        if (i == 4) break;
        
        string reward_type = _item_list[offset+i]["reward_type"].string_value() ;
        string reward_description;
        if(reward_type == "level_up_reward"){
            reward_description = "レベルアップ報酬" ;
        }else if(reward_type == "rank_up_reward"){
            reward_description = "ランクアップ報酬" ;
        }else if(reward_type == "quest_clear_reward"){
            reward_description = "ステージクリア報酬" ;
        }else if(reward_type == "event_clear_reward"){
            reward_description = "イベントドロップ報酬";
        }
        
        //緑色のテキスト
        auto *label_back = HMSFontUtil::createWithTTFBySystemFontArgument(reward_description, FONT_NAME_SYSTEM, 23);
        label_back->setHorizontalAlignment(TextHAlignment::CENTER);
        label_back->setTextColor(Color4B::GREEN);
        label_back->setPosition(Point(_item_sprite[offset+i]->getPosition().x + _item_sprite[offset+i]->getContentSize().width / 2 , _item_sprite[offset+i]->getPosition().y - _item_sprite[offset+i]->getContentSize().height / 2 - 20));
        _clipper->addChild(label_back,3,TAG_REWARD_ITEM) ;
        
        //白いテキスト(色の透過度を変化させてバックの緑色のテキストを隠したり表示したりすることで点滅を表現する)
        auto *label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_description, FONT_NAME_SYSTEM, 23);
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(_item_sprite[offset+i]->getPosition().x + _item_sprite[offset+i]->getContentSize().width / 2 , _item_sprite[offset+i]->getPosition().y - _item_sprite[offset+i]->getContentSize().height / 2 - 20));
        _clipper->addChild(label,4,TAG_REWARD_ITEM) ;
        
        //ラベルが点滅するアクション
        auto fadeTo1 = FadeTo::create(0.6, 0);
        auto fade_to_ease1 = EaseInOut::create(fadeTo1, 2);
        
        auto fadeTo2 = FadeTo::create(0.6, 255);
        auto fade_to_ease2 = EaseInOut::create(fadeTo2, 2);
        
        auto seq = Sequence::create(fade_to_ease1, fade_to_ease2, nullptr);
        auto rep = RepeatForever::create(seq);
        label->runAction(rep);
    }
}

void BootyPopupSprite::showBootySprites()
{
    //まとまりの数に応じてアイテムを生成
    for(int i=0 ; i<=_max_container_index; i++){
        showItemSprite(i); //アイテム画像の表示
        showItemName(i); //獲得アイテムの名前の表示
        showRewardDescription(i); //報酬の詳細説明を表示
    }
}

bool BootyPopupSprite::hasNextContainer()
{
    if(_now_container_index >= _max_container_index){
        return false;
    }else{
        return true;
    }
}

void BootyPopupSprite::slideNextContainer()
{
    //スワイプ中であればreturn
    if(_now_swipe)return ;
    _now_swipe = true ;
    
    for (auto node : _clipper->getChildren()) {
//        FadeTo *fade_to1 = FadeTo::create(0.1, 0) ;
//        DelayTime *fade_between_delay = DelayTime::create(0.3) ;
//        FadeTo *fade_to2 = FadeTo::create(0.1, 255) ;
//        Sequence *fade_seq = Sequence::create(fade_to1, fade_between_delay, fade_to2, nullptr) ;
//        
//        MoveBy *move_by = MoveBy::create(0.5, Point(-this->getContentSize().width,0)) ;
//        
//        EaseInOut *move_by_ease = EaseInOut::create(Spawn::createWithTwoActions(fade_seq, move_by), 2);
//        node->runAction(move_by_ease) ;
        
        MoveBy *move_by = MoveBy::create(0.5, Point(-this->getContentSize().width,0)) ;
        
        EaseInOut *move_by_ease = EaseInOut::create(move_by,2);
        node->runAction(move_by_ease) ;
    }
    
    //今表示されているまとまりのインデックスを移動後に進める
    CallFunc *func = CallFunc::create([this]() {
        _now_container_index++ ;
        _now_swipe = false;
    });
    this->runAction(Sequence::create(DelayTime::create(0.6f), func, NULL));
}

void BootyPopupSprite::onEnter()
{
    Sprite::onEnter();
    
    //ClippingNodeを作成
    createClippingNode() ;
    
    //アイテムリストの作成
    createItemList();
    
    //アイテム画像リストの作成
    createItemSprite();
    
    //アイテムの内容を表示
    showBootySprites() ;
    
}