#include "SkillConfirmLayer.h"
#include "HmsGeometricFunction.h"
#include "DungeonScene.h"
#include "ui/CocosGUI.h"
#include "Chara.h"
#include "CharaList.h"
#include "TurnManage.h"

bool SkillConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("SkillConfirmLayer::init()");
	visible_flg = false;
    return true;
}

void SkillConfirmLayer::showDetail(Chara *used_chara)
{
    removeAllChildren();
    yes_btn = nullptr;
    
    //targeted_chara = nullptr;
    selectTargetFlg = false;
	visible_flg = true;
    ratio = getVisibleRatio();
	this->used_chara = used_chara;
	
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    dung->selectAbleEffect(false);//配置可能キャラが光る表示を停止
    
	//スキル使用キャラの選択表示
	chara_select = Sprite::create("download/Dungeon/battle_skill_sellect.png");
	chara_select->setPosition(this->used_chara->getPosition());
	chara_select->setScale(ratio * 2);
	this->addChild(chara_select, 2, SCL_CHARA_SELECT_TAG);
	
	//下画面の表示
    footer_bg_img = nullptr;
    footer_bg_img = Sprite::create("init/Dungeon/design/battle_skill_popup.png");
    this->addChild(footer_bg_img, 2, SCL_FOOTER_TAG);
    footer_bg_img->setOpacity(255);
    footer_bg_img->setScale(ratio);
    footer_bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2 * -1, DISPLAY_HEIGHT /2 - 370), 0.5, 0.5));
    
    
	//下画面のテキスト
	Label* footer_text = HMSFontUtil::createWithTTFBySystemFontArgument("スキルを発動しますか？", FONT_NAME_SYSTEM, 33);
	footer_text->setPosition(Point(footer_bg_img->getContentSize().width / 2, footer_bg_img->getContentSize().height - 65));
	footer_bg_img->addChild(footer_text, 2, SCL_FOOTER_TAG);
	
	
	Sprite* yes_off = Sprite::create("init/Home/Common/bt_yes.png");
	Sprite* yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B::GRAY);
    
    
    //『はい』ボタン
    yes_btn = MenuItemSprite::create(yes_off, yes_on, [&](Ref* pSender){
        skillPrepare();
    });
    yes_btn->setPosition(Point((footer_bg_img->getContentSize().width / 2) - (yes_btn->getContentSize().width / 2) - 10, (yes_btn->getContentSize().height / 2) + 10));
    
    //『いいえ』ボタン
    MenuItemImage *pNoItem = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/bt_no.png", [&](Ref* pSender){
        close();
    });
    pNoItem->setPosition(Point((footer_bg_img->getContentSize().width / 2) + (pNoItem->getContentSize().width / 2) + 10, (pNoItem->getContentSize().height / 2) + 10));
    
    Menu* pMenu = Menu::create(yes_btn, pNoItem, NULL);
    pMenu->setPosition( Point::ZERO );
    footer_bg_img->addChild(pMenu, 2, SCL_FOOTER_YES_TAG);
    

    //ヘッダーの表示
    header_bg_img = Sprite::create("init/Dungeon/design/battle_skill_bk_player_as.png");
    header_bg_img->setScale(ratio);
    header_bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2 * 2, DISPLAY_HEIGHT /2 + 50), 0.5, 0.5));
    this->addChild(header_bg_img, 2, SCL_HEADER_TAG);
    
    //ヘッダーの表示（パッシブ）
    header_bg_img_passive = nullptr;
    if(used_chara->getPassiveSkillId()>0) {
        header_bg_img_passive = Sprite::create("init/Dungeon/design/battle_skill_bk_player_ps.png");
        header_bg_img_passive->setScale(ratio);
        header_bg_img_passive->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2 * 2, DISPLAY_HEIGHT /2 + 50 + 160), 0.5, 0.5));
        this->addChild(header_bg_img_passive, 2, SCL_HEADER_P_TAG);
        
        //スキル名
        skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(used_chara->getPassiveSkillName(), FONT_NAME_SYSTEM, 28);
        skill_name->setAnchorPoint(Point(0, 0.5));
        skill_name->setPosition(Point(header_bg_img->getContentSize().width / 2 + 20, header_bg_img_passive->getContentSize().height - 50));
        skill_name->setColor(Color3B::ORANGE);//オレンジ色
        header_bg_img_passive->addChild(skill_name, 3, SCL_SKILL_NAME_TAG);
        
        //スキル説明
        skill_detail = HMSFontUtil::createWithTTFBySystemFontArgument(used_chara->getPassiveSkillDetail(), FONT_NAME_SYSTEM, 24);//全角38文字が限界です。
        skill_detail->setDimensions(460, 60 );
        skill_detail->setAnchorPoint(Point(0, 1.0));
        skill_detail->setPosition(Point(156, header_bg_img_passive->getContentSize().height - 70));
        header_bg_img_passive->addChild(skill_detail, 3, SCL_SKILL_DETAIL_TAG);
        
        //スキルコスト
        skill_cost_sprite.clear();
        int skill_cost = 0;
        int fig2       = floor(skill_cost/10.0f);
        int fig1       = skill_cost % 10;
        
        if(fig2==0){//1桁
            skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig1) + ".png"));
            skill_cost_sprite.front()->setScale(2.0f);
            skill_cost_sprite.front()->setPosition(Point(78, header_bg_img_passive->getContentSize().height / 2 + skill_cost_sprite.front()->getContentSize().height / 2 - 35));
            header_bg_img_passive->addChild(skill_cost_sprite.front(), 2, SCL_SKILL_COST_TAG);
        }else{//2桁
            skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig1) + ".png"));
            skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig2) + ".png"));
            int skill_cost_width = 95;
            int skill_cost_height = header_bg_img_passive->getContentSize().height / 2 + skill_cost_sprite.front()->getContentSize().height / 2 + 8;
            
            for(int i=0; i<skill_cost_sprite.size(); i++){
                skill_cost_sprite.at(i)->setScale(2.0f);
                skill_cost_sprite.at(i)->setPosition(Point(skill_cost_width - (35 * i), skill_cost_height - 35));
                header_bg_img_passive->addChild(skill_cost_sprite.at(i), 2, SCL_SKILL_COST_TAG);
            }
        }

    }

	
	//スキル名
	skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(used_chara->getActiveSkillName(), FONT_NAME_SYSTEM, 28);
	skill_name->setAnchorPoint(Point(0, 0.5));
	skill_name->setPosition(Point(header_bg_img->getContentSize().width / 2 + 20, header_bg_img->getContentSize().height - 50));
	skill_name->setColor(Color3B::ORANGE);//オレンジ色
	header_bg_img->addChild(skill_name, 3, SCL_SKILL_NAME_TAG);
	
	//スキル説明
	skill_detail = HMSFontUtil::createWithTTFBySystemFontArgument(used_chara->getActiveSkillDetail(), FONT_NAME_SYSTEM, 24);//全角38文字が限界です。
    skill_detail->setDimensions(460, 60 );
	skill_detail->setAnchorPoint(Point(0, 1.0));
	skill_detail->setPosition(Point(156, header_bg_img->getContentSize().height - 70));
	header_bg_img->addChild(skill_detail, 3, SCL_SKILL_DETAIL_TAG);
	
	//スキルコスト
	skill_cost_sprite.clear();
	int skill_cost = used_chara->getActiveSkillCost();
	int fig2       = floor(skill_cost/10.0f);
	int fig1       = skill_cost % 10;
	
	if(fig2==0){//1桁
		skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig1) + ".png"));
		skill_cost_sprite.front()->setScale(2.0f);
		skill_cost_sprite.front()->setPosition(Point(78, header_bg_img->getContentSize().height / 2 + skill_cost_sprite.front()->getContentSize().height / 2 - 35));
		header_bg_img->addChild(skill_cost_sprite.front(), 2, SCL_SKILL_COST_TAG);
	}else{//2桁
		skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig1) + ".png"));
		skill_cost_sprite.push_back(Sprite::create("init/Dungeon/design/battle_num_w" + to_string(fig2) + ".png"));
		int skill_cost_width = 95;
		int skill_cost_height = header_bg_img->getContentSize().height / 2 + skill_cost_sprite.front()->getContentSize().height / 2 + 8;
		
		for(int i=0; i<skill_cost_sprite.size(); i++){
			skill_cost_sprite.at(i)->setScale(2.0f);
			skill_cost_sprite.at(i)->setPosition(Point(skill_cost_width - (35 * i), skill_cost_height - 35));
			header_bg_img->addChild(skill_cost_sprite.at(i), 2, SCL_SKILL_COST_TAG);
		}
	}
	
	//スキル詳細のアニメーション(header)
	header_bg_img->runAction(
							 Sequence::create(MoveTo::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT /2 + 50), 0.5, 0.5)),
											  ScaleTo::create(0.1f, 1.1f * ratio),
											  ScaleTo::create(0.1f, 1.0f * ratio),
											  nullptr
											  )
							 );
    
    if(header_bg_img_passive!=nullptr) {
        //スキル詳細のアニメーション(header)
        header_bg_img_passive->runAction(
                                 Sequence::create(MoveTo::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT /2 + 50 + 160), 0.5, 0.5)),
                                                  ScaleTo::create(0.1f, 1.1f * ratio),
                                                  ScaleTo::create(0.1f, 1.0f * ratio),
                                                  nullptr
                                                  )
                                 );
    }
    
	//スキル詳細のアニメーション(footer)
	footer_bg_img->runAction(
							 Sequence::create(MoveTo::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT /2 - 370), 0.5, 0.5)),
											  ScaleTo::create(0.1f, 1.1f * ratio),
											  ScaleTo::create(0.1f, 1.0f * ratio),
											  nullptr
											  )
							 );

	//キャラ周りの青い枠のアニメーション
	chara_select->runAction(
							Sequence::create(ScaleTo::create(0.1f, 1.0f * ratio),
											 DelayTime::create(0.2f),
											 nullptr
											 )
							);
    
    
    //イベントリスナーの作成
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SkillConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SkillConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SkillConfirmLayer::onTouchEnded, this);
    
    listener->setSwallowTouches(true);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
}

void SkillConfirmLayer::skillPrepare()
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    targeted_index = 0;
    target_charas.clear();
    target_places.clear();
    
    if(dung->isMyTurn() && dung->getState() == "Operationing" && dung->turn_change_wait == false){//自分のターンでプレイスキャラクターの時のみ実行
        HmsVector<string> target_types = split(used_chara->getActiveSkillTarget(), "#");
        
        if(target_types.size() >= 4 && target_types.at(3) == "select"){//キャラクター選択式すきる
            target_charas = dung->activeSkill->getTargetCharas(used_chara, (CharaList::getAll()).getVector(), target_types.at(0));//対象のキャラ
            if(target_charas.empty()){//対象がいない場合
                skill();//エラー表示
            }else{//対象がいる時
                selectTarget();//選択画面へ
            }
            
        }else if(target_types.size() >= 4 && target_types.at(3) == "place"){//場所選択式すきる
            target_places = dung->activeSkill->getTargetPlace(used_chara, target_types.at(0));
            if(target_places.empty()){//対象がいない場合
                skill();//エラー表示
            }else{//対象がいる時
                selectPlace();//選択画面へ
            }
            
        }else{
            skill();
        }
    }
}

void SkillConfirmLayer::skill()
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    for(Chara *chara : CharaList::getAll()){
        chara->charaNameTab(false);//名前表示の消去
    }
    
    if(target_charas.empty()==false){//ターゲットキャラがいる場合
        CCLOG("選択対象変更プライマリ2:%d,フィールド：%d",target_charas.at(targeted_index)->getPrimaryId(),target_charas.at(targeted_index)->getFieldIndex());
        dung->useSkill(used_chara->getPrimaryId(),to_string(target_charas.at(targeted_index)->getPrimaryId()) + "&1");
        
    }else if(target_places.empty()==false){//ターゲットプレイスがある場合
        //ここから
        dung->useSkill(used_chara->getPrimaryId(),to_string(used_chara->getPrimaryId()) + "&" +to_string(target_places.at(targeted_index).first) +"&" +to_string(target_places.at(targeted_index).second));
        
    }else{//いませんエラー出す場合
        dung->useSkill(used_chara->getPrimaryId(),"");
    }
    
    listener->setSwallowTouches(false);
    listener->setEnabled(false);
    dung->selectAbleEffect(false);
    
    end();
}

void SkillConfirmLayer::selectTarget(){//対象選択スキル用画面
    selectTargetFlg = true;
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    vector<Point> field_place_vec;
    for(Chara *chara : target_charas){
        field_place_vec.push_back(chara->getPosition());
    }
    
    dung->cardLayer->whiteFilterCreate(field_place_vec);//ターゲットの場所を点滅
    
    for(int i=0; i<target_charas.size(); i++){
        if(targeted_index == 0 || target_charas.at(targeted_index)->getFieldIndex() < target_charas.at(i)->getFieldIndex()){
            targeted_index = i;//現在選択中のキャラクタ
        }
    }

    makeSelectUI(target_charas.at(targeted_index)->getPosition());
}

void SkillConfirmLayer::selectPlace(){//対象選択スキル用画面
    selectTargetFlg = true;
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    vector<Point> field_place_vec;
    for(pair<int, int> pla : target_places){
        field_place_vec.push_back(dung->fieldLayer->field_position_list[pla.first].at(pla.second));
    }
    
    dung->cardLayer->whiteFilterCreatePlace(target_places);//ターゲットの場所を点滅
    
    makeSelectUI(field_place_vec.at(0));
}

void SkillConfirmLayer::makeSelectUI(Point mark_position){//対象選択のUI作成
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    if(target_mark == nullptr){
        target_mark = Sprite::create("download/Dungeon/target.png");
        addChild(target_mark,40000);
    }
    target_mark->setScale(ratio);
    target_mark->setPosition(mark_position);
    target_mark->setRotation(-60);
    auto smoll = ScaleTo::create(0.1f, ratio * 3/4);
    auto big = ScaleTo::create(0.1f, ratio);
    auto seq = Sequence::create(smoll,big, NULL);
    auto naname = RotateBy::create(0.2f, 60);
    auto spa = Spawn::create(seq,naname, NULL);
    target_mark->runAction(spa);
    
    
    auto move = MoveTo::create(0.25f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 110), 0.5, 0.5));
    auto ease = EaseSineInOut::create(move);
    header_bg_img->runAction(ease);
    
    if(header_bg_img_passive) header_bg_img_passive->setVisible(false);
    footer_bg_img->setVisible(false);
    
    
    //『発動』ボタン
    hatudouItem = MenuItemImage::create("download/Dungeon/bt_motion.png", "download/Dungeon/bt_motion.png", [&,dung](Ref* pSender){
        if(dung->isMyTurn() && dung->getState() == "Operationing" && dung->turn_change_wait == false){//自分のターンでプレイスキャラクターの時のみ実行
            skill();
        }
    });
    hatudouItem->setPosition(-convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8), 0.5, 0.5).x / 3, 0);
    hatudouItem->setScale(ratio);
    
    //『キャンセル』ボタン
    cancelItem = MenuItemImage::create("download/Dungeon/bt_cancel.png", "download/Dungeon/bt_cancel.png", [&](Ref* pSender){
        close();
    });
    cancelItem->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8), 0.5, 0.5).x / 3, 0);
    cancelItem->setScale(ratio);
    
    
    Menu* pMenu = Menu::create(hatudouItem,cancelItem, NULL);
    pMenu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8), 0.5, 0.5));
    //pMenu->setScale(ratio);
    addChild(pMenu, 22);
    
    
    //対象選択の後ろのやつ
    auto mid_text_back = Sprite::create("download/Dungeon/bk_target.png");
    mid_text_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    mid_text_back->setScale(ratio);
    this->addChild(mid_text_back, 2, SCL_FOOTER_TAG);
    
    
    Label* mid_text = HMSFontUtil::createWithTTFBySystemFontArgument("スキルの対象を選択してください", FONT_NAME_SYSTEM, 35*ratio);
    mid_text->setColor(Color3B::WHITE);
    mid_text->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    mid_text->setGlobalZOrder(1.0f);
    this->addChild(mid_text, 2, SCL_FOOTER_TAG);
    //auto ease1 = EaseSineInOut::create(Spawn::create(FadeTo::create(1.5f, 150),ScaleTo::create(1.5f, 1.05f), NULL));
    //auto ease2 = EaseSineInOut::create(Spawn::create(FadeTo::create(1.5f, 255),ScaleTo::create(1.5f, 1.0f), NULL));
    //auto seq = Sequence::create(ease1,ease2, NULL);
    //mid_text->runAction(RepeatForever::create(seq));
    
    //影
    shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setOpacity(0);
    shadow->runAction(FadeTo::create(0.15f, 255));
    
    auto clipper = ClippingNode::create();//わかんねーーーー
    clipper->setAnchorPoint(Vec2(0.5f,0.5f));
    clipper->setInverted(true);
    clipper->setAlphaThreshold(0.22f);//これいれないと動かない・・・
    
    auto render = RenderTexture::create(getContentSize().width,getContentSize().height);//四角スプライトをまとめる
    
    render->setAnchorPoint(Vec2(0.5f,0.5f));
    render->begin();//書き込み開始
    
    if(target_charas.empty() == false){
        for(Chara *chara : target_charas){
            Sprite* square = Sprite::create();
            square->setTextureRect(Rect(0,0,100,100));
            square->setAnchorPoint(Vec2(0.5f,0.5f));
            square->setScale(ratio);
            square->setPosition(dung->fieldLayer->getCharaPosition(chara));
            square->visit();
        }
    }else{
        for(pair<int,int> place : target_places){//ターゲットの所の穴
            Sprite *img;
            if(place.first == TurnManage::getEnemyIndex()){//敵の場所の
                if(place.second == 1){//ここ大丈夫か確認
                    img = Sprite::create("download/Dungeon/white_e_left.png");
                    img->setPosition(convertFromDisplaySize(Point(195, 707), 0.5, 0.5));
                }else if(place.second == 0){
                    img = Sprite::create("download/Dungeon/white_e_right.png");
                    img->setPosition(convertFromDisplaySize(Point(447, 707), 0.5, 0.5));
                }else{
                    img = Sprite::create("download/Dungeon/white.png");
                    img->setPosition(convertFromDisplaySize(Point(571 - (place.second-2)*100, 576), 0.5, 0.5));
                }
            }else{//味方の場所の
                if(place.second == 1){
                    img = Sprite::create("download/Dungeon/white_p_left.png");
                    img->setPosition(convertFromDisplaySize(Point(195, 254), 0.5, 0.5));
                }else if(place.second == 0){
                    img = Sprite::create("download/Dungeon/white_p_right.png");
                    img->setPosition(convertFromDisplaySize(Point(447, 254), 0.5, 0.5));
                }else{
                    img = Sprite::create("download/Dungeon/white.png");
                    img->setPosition(convertFromDisplaySize(Point(71 + (place.second-2)*100, 392), 0.5, 0.5));
                }
            }
            img->setAnchorPoint(Vec2(0.5f,0.5f));
            img->setScale(ratio);
            img->visit();
        }
    }
    
    Sprite* square = Sprite::create();//スキル使ったキャラの周りの穴
    square->setTextureRect(Rect(0,0,120,120));
    square->setAnchorPoint(Vec2(0.5f,0.5f));
    square->setScale(ratio);
    square->setPosition(dung->fieldLayer->getCharaPosition(used_chara));
    square->visit();
    
    
    render->end();//書き込み終了
    
    clipper->addChild(shadow);
    clipper->setStencil(render->getSprite());
    clipper->setAnchorPoint(Vec2(0.5f,0.5f));
    clipper->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    addChild(clipper,1);
    
}

void SkillConfirmLayer::close()
{
    if(target_mark != nullptr) target_mark->setVisible(false);
    target_mark = nullptr;
    if(yes_btn != nullptr) yes_btn->setEnabled(false);//ボタン無効化
    if(hatudouItem != nullptr) hatudouItem->setVisible(false);
    if(cancelItem != nullptr) cancelItem->setVisible(false);
    hatudouItem = nullptr;
    cancelItem = nullptr;
    
    if(listener != nullptr){
        listener->setSwallowTouches(false);
        listener->setEnabled(false);
    }
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    dung->selectAbleEffect();//配置可能キャラが光る表示を再開
    
    for(Chara *chara : CharaList::getAll()){
        chara->charaNameTab(false);//名前表示の消去
    }
    
    if(shadow!=nullptr){
        shadow->runAction(FadeTo::create(0.15f, 0));
        shadow = nullptr;
    }
    
    
	auto func = CallFunc::create([&,this](){
		static_cast<DungeonScene *>(getParent())->selectAbleEffect();
        this->end();
	});
    
    
	//スキル詳細のアニメーション
    if(header_bg_img!=nullptr) {
        header_bg_img->runAction(
                                 Sequence::create(MoveBy::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH  * -1, 0), 0.5, 0.5)),
                                                  ScaleTo::create(0.1f, 0.0f * ratio),
                                                  FadeOut::create(0.1f),
                                                  RemoveSelf::create(),
                                                  nullptr
                                                  )
                                 );
    }
    if(header_bg_img_passive!=nullptr) {
        //スキル詳細のアニメーション
        header_bg_img_passive->runAction(
                                 Sequence::create(MoveBy::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH  * -1, 0), 0.5, 0.5)),
                                                  ScaleTo::create(0.1f, 0.0f * ratio),
                                                  FadeOut::create(0.1f),
                                                  nullptr
                                                  )
                                 );
    }
    if(footer_bg_img!=nullptr) {
        footer_bg_img->runAction(
                                 Sequence::create(MoveBy::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH  * 1, 0), 0.5, 0.5)),
                                                  ScaleTo::create(0.1f, 0.0f * ratio),
                                                  FadeOut::create(0.1f),
                                                  nullptr
                                                  )
                                 );
    }
    //キャラ周りの青い枠のアニメーション
    if(chara_select!=nullptr) {
        chara_select->runAction(
                                Sequence::create(ScaleTo::create(0.1f, 1.1f * ratio),
                                                 FadeOut::create(0.1f),
                                                 func,
                                                 nullptr
                                                 )
                                );
    }
}

void SkillConfirmLayer::end(){
    target_mark = nullptr;
    this->removeAllChildren();
    visible_flg = false;
    yes_btn = nullptr;
    shadow = nullptr;
    hatudouItem = nullptr;
    cancelItem = nullptr;
    //targeted_chara = nullptr;
    header_bg_img = nullptr;
    header_bg_img_passive = nullptr;
    footer_bg_img = nullptr;
    chara_select = nullptr;
}

//タップ処理
bool SkillConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    if(selectTargetFlg == true){//ターゲット選択画面の時
        Point touch_point = touch->getLocation();
        
        if(target_charas.empty()==false){//キャラクター選択型スキル
            
            for(int i=0; i<target_charas.size(); i++){
                
                if((target_charas.at(i)->isOnField() && target_charas.at(i)->getFieldIndex() == dung->fieldLayer->getPositionIndex(touch_point, "field", target_charas.at(i)->getBelongingIndex()))
                   || (target_charas.at(i)->getType()==CHARA_TYPE_LEADER && dung->fieldLayer->getPositionIndex(touch_point, "leader", target_charas.at(i)->getBelongingIndex()))){//このキャラがタッチされた時
                    //CCLOG("選択対象変更プライマリ1:%d,フィールド：%d",chara->getPrimaryId(),chara->getFieldIndex());
                    targeted_index = i;
                    target_mark->setPosition(target_charas.at(i)->getPosition());
                    target_mark->setRotation(-60);
                    auto smoll = ScaleTo::create(0.1f, ratio * 3/4);
                    auto big = ScaleTo::create(0.1f, ratio);
                    auto seq = Sequence::create(smoll,big, NULL);
                    auto naname = RotateBy::create(0.2f, 60);
                    auto spa = Spawn::create(seq,naname, NULL);
                    target_mark->runAction(spa);
                    break;
                }
            }
        }else{//場所選択型スキル
            
            for(int i=0; i<target_places.size(); i++){
                Point mid_point = dung->fieldLayer->field_position_list[target_places.at(i).first].at(target_places.at(i).second);
                
                if((mid_point.x - 50*ratio)<=touch_point.x && (mid_point.x + 50*ratio)>=touch_point.x && (mid_point.y - 50*ratio)<=touch_point.y && (mid_point.y + 50*ratio)>=touch_point.y){//その場所の中をタッチしたら
                    targeted_index = i;
                    target_mark->setPosition(mid_point);
                    target_mark->setRotation(-60);
                    auto smoll = ScaleTo::create(0.1f, ratio * 3/4);
                    auto big = ScaleTo::create(0.1f, ratio);
                    auto seq = Sequence::create(smoll,big, NULL);
                    auto naname = RotateBy::create(0.2f, 60);
                    auto spa = Spawn::create(seq,naname, NULL);
                    target_mark->runAction(spa);
                    break;
                }
            }
        }
    }
    return true;
}

void SkillConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
   
}

void SkillConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(selectTargetFlg == false){//普通の画面の時
        Rect rect = this->footer_bg_img->getBoundingBox();
        if(rect.containsPoint(touch->getLocation()) == false) {//枠の外をクリック
            this->close();
            
        }
    }
    
}

