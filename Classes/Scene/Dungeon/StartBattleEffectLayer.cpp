
#include "StartBattleEffectLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "SimpleAudioEngine.h"
#include "HmsUtilityFunction.h"
#include "Effect.h"
#include "TurnManage.h"
#include "CharaList.h"
#include "PlistAnimationSprite.h"

bool StartBattleEffectLayer::init()
{
    if ( !Layer::init() ) {
        return false;
    }
    ratio = getVisibleRatio();
    return true;
}
void StartBattleEffectLayer::initialize()
{
   
    
    
    
    
}
void StartBattleEffectLayer::startBattleEffect(Layer* parent, SEL_SCHEDULE s, Json json_master)
{
    
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    /*listener->onTouchBegan = [s, parent, this](Touch* touch, Event* event){
        //もしレイヤーがタップされた場合その場で削除する
        parent->scheduleOnce(s, 0.0f);
        this->removeFromParent() ;
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    */
    //画面の中心
    Point center_point = Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2) ;
    
    //背景の影のエフェクトの生成と再生
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio) ;
    shadow->setPosition(convertFromDisplaySize(center_point, 0.5, 0.5)) ;
    this->addChild(shadow, ORDER_SHADOW) ;
    
    DelayTime *delay = DelayTime::create(4.75f) ;
    FadeOut *fadeOut = FadeOut::create(0.25f) ;
    RemoveSelf* remove = RemoveSelf::create(true);
    CallFunc *func1 = CallFunc::create([s,parent,this,listener]() {
        auto dispa = Director::getInstance()->getEventDispatcher();
        dispa->removeEventListener(listener);
        parent->scheduleOnce(s, 0.0f);
    });//ラムダ式
    DelayTime *delay1 = DelayTime::create(1.0f) ;
    CallFunc *func1_1 = CallFunc::create([this]() {
        this->removeFromParent() ;
    });//ラムダ式
    
    
    shadow->runAction(Sequence::create(delay, fadeOut, remove, func1, delay1, func1_1, nullptr)) ;
    
    makeUserDetailEffect(json_master) ;
    
    //スプライトアニメーションとSEの生成と再生
    DelayTime *delay3 = DelayTime::create(1.0f) ;
    CallFunc *func3 = CallFunc::create([this,center_point]() {
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/06_vs.mp3") ;
    });//ラムダ式
    DelayTime *delay2 = DelayTime::create(0.5f) ;
    CallFunc *func2 = CallFunc::create([this,center_point]() {
        //スプライトシートアニメーション 雷
        Effect *thunder = Effect::create() ;
        thunder->setData(this, "download/Effect/Sprite/thunder_vs", 4, convertFromDisplaySize(center_point, 0.5, 0.5), 1, ORDER_SPRITE_ANIM_THUNDER, 22) ;
        
        DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
        if(dungeon_scene->getBattleMode()=="vs" && SaveData::loadBool("revenge_mode")==true && dungeon_scene->isTournamentEventBattle() == false
           && SaveData::loadString("online_battle_type")=="quick") {
            auto revenge_sprite = PlistAnimationSprite::setData((Layer *)this, "download/Effect/Sprite/ef_revenge3", 1, 0.2f, 10);
            revenge_sprite->setPosition(convertFromDisplaySize(center_point, 0.5, 0.5));
            revenge_sprite->setScale(2.0*ratio);
        } else {
            //スプライトシートアニメーション 開戦の文字
            Effect *word = Effect::create() ;
            word->setData(this, "download/Effect/Sprite/start", 90, convertFromDisplaySize(center_point, 0.5, 0.5), 1, ORDER_SPRITE_ANIM_WORD) ;
        }
    });//ラムダ式
    runAction(Sequence::create(delay3, func3, delay2, func2, nullptr)) ;
    
}

void StartBattleEffectLayer::makeUserDetailEffect(Json json_master){
    
    int ahead_no = json_master["ahead_no"].int_value();
    if(ahead_no==0){
        ahead_no = stoi(json_master["ahead_no"].string_value());//応急処置
    }
    
    int ahead_index = ahead_no - 1 ;
    HMSLOG("アヘッドナンバー%d",ahead_no);
    
    //画面の中心
    Point center_point = Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2) ;
    
    float card_size_ratio = 1.52 ; //カードの拡大倍率
    
    // 画面の端と中心の差を想定画面サイズ上の数値に変換して取得する(DISPLAY_WIDTH/2と異なり縦横比によって生じる左右の余白を加味したサイズ)
    int display_width = _contentSize.width/ratio ;
    
    //プレイヤーカードのエフェクト中の初期位置・途中位置・終端位置の中心位置からの差(プレイヤー側)　三箇所
    Point start_point_diff = Point(-display_width, -300) ;
    Point center_point_diff = Point(0, -300 ) ;
    Point end_point_diff = Point(display_width, -300) ;
   
    for(int ii=0; ii<2; ii++) {
        
        //カードのエフェクト中の初期位置・途中位置・終端位置 (プレイヤーとエネミーは常に点対称の位置でアニメーションする)
        Point start_point ;
        Point stop_point ;
        Point end_point ;
        
        if(ii == TurnManage::getPlayerIndex()) {
            start_point = center_point + start_point_diff ;
            stop_point = center_point + center_point_diff ;
            end_point = center_point + end_point_diff ;
        } else {
            start_point = center_point - start_point_diff ;
            stop_point = center_point - center_point_diff ;
            end_point = center_point - end_point_diff ;
        }
        
        Sprite *window_sprite; //外枠のスプライト 先攻か後攻か ahead_indexの値で切り替える
        if(ahead_index==ii) window_sprite = Sprite::create("init/Dungeon/design/before_battle1.png") ;
        else window_sprite = Sprite::create("init/Dungeon/design/before_battle2.png") ;
        window_sprite->setScale(ratio) ;
        
        //カードを初期位置に設定
        window_sprite->setPosition(convertFromDisplaySize(start_point, 0.5, 0.5)) ;
        
        //カード画像の初期化 start
        Sprite *card_sprite = Sprite::create() ;
        card_sprite->setAnchorPoint(Point(0.5f, 0.5f)) ;
        card_sprite->setContentSize(Size(100, 100)) ;
        card_sprite->setScale(card_size_ratio) ;
        card_sprite->setPosition(Point(110,100)) ;
        
        //背景画像のコピーと追加
        Sprite *back_sprite = Sprite::createWithSpriteFrame(Sprite::create("download/Chara/parts400/card_bk" + to_string(CharaList::getLeader(ii)->getRare()) + ".png")->displayFrame()) ;
        back_sprite->setScale(0.25f);
        back_sprite->setAnchorPoint(Point::ZERO) ;
        back_sprite->setPosition(Point::ZERO) ;
        card_sprite->addChild(back_sprite, 1) ;
        
        //キャラ画像のコピーと追加
        Sprite *chara_sprite = Sprite::createWithSpriteFrame(Sprite::create("download/Chara/chara400/" + to_string(CharaList::getLeader(ii)->getCharaId()) + ".png")->displayFrame()) ;
        chara_sprite->setScale(0.25f) ;
        chara_sprite->setAnchorPoint(Point::ZERO) ;
        chara_sprite->setPosition(Point::ZERO) ;
        card_sprite->addChild(chara_sprite, 2) ;
        
        //レアリティ画像のコピーと追加
        Sprite *rare_sprite = Sprite::createWithSpriteFrame(Sprite::create("download/Chara/parts400/card_rare" + to_string(CharaList::getLeader(ii)->getRare()) + ".png")->displayFrame()) ;
        rare_sprite->setScale(0.25f) ;
        rare_sprite->setAnchorPoint(Point::ZERO) ;
        rare_sprite->setPosition(Point::ZERO) ;
        card_sprite->addChild(rare_sprite, 3) ;
        
        //カード画像の初期化 end
        window_sprite->addChild(card_sprite) ;
        
        
        //レベル
        string level = json_master["player_data" + to_string(ii+1)]["level"].string_value() ;
        if(level=="0" || level == ""){
            //レベルが無い場合レベルに関して表示しない
        }else{
            //文字「Lv.」
            auto lavel_lv = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 16);
            lavel_lv->setAnchorPoint(Point(0, 0.5));
    		lavel_lv->setColor(Color3B(255, 204, 0));
            lavel_lv->setPosition(Point(200, 170));
    //enableOutline(Color4B(0, 0, 0, 255), 1);
            window_sprite->addChild(lavel_lv) ;
            
            auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(level, FONT_NAME_SYSTEM, 24);
            level_label->setAnchorPoint(Point(0, 0.5));
    		level_label->setColor(Color3B(255, 204, 0));
            level_label->setPosition(Point(230, 170));
    //enableOutline(Color4B(0, 0, 0, 255), 1);
            window_sprite->addChild(level_label) ;
        }
        
        //階級名
        string rank_name = json_master["player_data" + to_string(ii+1)]["rank_name"].string_value() ;
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_name, FONT_NAME_SYSTEM, 24);
        rank_label->setAnchorPoint(Point(0, 0.5));
		rank_label->setColor(Color3B(255, 204, 0));
        rank_label->setPosition(Point(200, 145));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        window_sprite->addChild(rank_label) ;
        
        
        //ユーザー名
        string player_name = json_master["player_data" + to_string(ii+1)]["player_name"].string_value() ;
        Label *name_label = HMSFontUtil::createWithTTFBySystemFontArgument(player_name, FONT_NAME_SYSTEM, 30);
        name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        name_label->setPosition(Point(356, 143));
        name_label->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        window_sprite->addChild(name_label, 2);
        
        
        //コメント
        string comment = json_master["player_data" + to_string(ii+1)]["comment"].string_value() ;
        Label *comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(comment, FONT_NAME_SYSTEM, 24);
        comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
        comment_label->setDimensions(370, 100);
        comment_label->setAnchorPoint(Point(0, 1.0f));
        comment_label->setPosition(Point(212, 119));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        window_sprite->addChild(comment_label, 10);
        
        
        //カードがスライドして登場するアクションの生成
        MoveTo *move_center = MoveTo::create(0.25f, convertFromDisplaySize(stop_point, 0.5, 0.5));
        EaseBackOut *ease_out = EaseBackOut::create(move_center);
    
        //カードがスライドして去るアクションの生成
        MoveTo *move_end = MoveTo::create(0.25f, convertFromDisplaySize(end_point, 0.5, 0.5));
        EaseBackIn *ease_in = EaseBackIn::create(move_end);
//        EaseIn *ease_in = EaseIn::create(move_end, 4);
        
        //SEの生成と再生
        CallFunc *func = CallFunc::create([this,center_point]() {
            //カードシャッフル効果音
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/card_tap.mp3",false);
        });//ラムダ式
        DelayTime *delay1 = DelayTime::create(1.5f) ;
        DelayTime *delay2 = DelayTime::create(4.0f) ;
        this->addChild(window_sprite, ORDER_CARD) ;
        
        RemoveSelf* remove2 = RemoveSelf::create(true);
//        window_sprite->runAction(Sequence::create(delay1, ease_out, delay2,func, ease_in, remove2, nullptr)) ;
        window_sprite->runAction(Sequence::create(delay1, ease_out, delay2, ease_in, remove2, nullptr)) ;
    }
}
