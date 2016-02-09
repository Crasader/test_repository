#include "BattleTutorialBeforeLayer.h"
#include "TutorialBattleScene.h"

bool BattleTutorialBeforeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleTutorialBeforeLayer::init()");
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _user_name   = SaveData::loadString("user_name");
    
    return true;
}

void BattleTutorialBeforeLayer::createMessage()
{
    //キャラ画像作成
    message_talk_sprite = MessageTalkSprite::create() ;
    message_talk_sprite->setScale(ratio);
    message_talk_sprite->setPosition(convertFromDisplaySize(Point(320, 580), 0.5, 0.5));
    addChild(message_talk_sprite, 1, MESSAGE_SPRITE_TAG);
    
}
void BattleTutorialBeforeLayer::initEventListener()
{
    listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = CC_CALLBACK_2(BattleTutorialBeforeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleTutorialBeforeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleTutorialBeforeLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool BattleTutorialBeforeLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(message_talk_sprite->isTalkWait()){
    }else if(message_talk_sprite->isTalkNow()){
        //トーク中にタップした場合全てのコメントを強制的に表示する
        message_talk_sprite->forceAllTalk() ;
    }else if(message_talk_sprite->isTalkEnd()){
        if(step_state != 3)message_talk_sprite->talkWait() ;
        nextStep(0.0f);
    }
    return true;
}
void BattleTutorialBeforeLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    
}
void BattleTutorialBeforeLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void BattleTutorialBeforeLayer::changeSceneToDungeonScene()
{
    //チュートリアルステータスの変更
    SaveData::saveInt("tutorialStatus", 1);
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //バトルチュートリアルページへの遷移
    auto scene = DungeonScene::createScene();
    auto transition = TransitionFade::create(1.0f, scene);
    Director::getInstance()->replaceScene(transition);
}

void BattleTutorialBeforeLayer::onEnter()
{
    Layer::onEnter();
    
    step_state = 0 ;
    
    //メッセージの作成
    createMessage();
    
    //リスナーの初期化
    initEventListener() ;

    //SE
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/navi_open.mp3",false);

    this->scheduleOnce(schedule_selector(BattleTutorialBeforeLayer::nextStep), 2.0f);
}

void BattleTutorialBeforeLayer::nextStep(float dt){
    
    if(step_state == 0){
        message_talk_sprite->talkStart(_user_name + "さん、はじめまして！\nこのゲームを案内する貂蝉です！") ;
        step_state++ ;
    }else if(step_state == 1){
        message_talk_sprite->talkStart("三国志バトルウォーズは\n自分だけのオリジナルデッキを作り\nライバルと戦う、カードゲームです♪") ;
        step_state++ ;
    }else if(step_state == 2){
        message_talk_sprite->talkStart("早速、バトルの\nチュートリアルを始めましょう♪") ;
        step_state++ ;
    }else if(step_state == 3){
        changeSceneToDungeonScene() ;
    }
        
}
