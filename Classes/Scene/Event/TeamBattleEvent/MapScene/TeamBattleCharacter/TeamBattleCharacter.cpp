#include "TeamBattleCharacter.h"
#include "HmsGeometricFunction.h"
#include "HMSFontUtil.h"
#include "MultiValue.h"
#include "PrimitiveParser.h"
#include "TeamBattleCharacterStatus.h"

using namespace std;

TeamBattleCharacter* TeamBattleCharacter::createLocal(Synchronizer *synchronizer){
    return create(synchronizer, true, "");
}
TeamBattleCharacter* TeamBattleCharacter::createPrototype(Synchronizer *synchronizer){
    return create(synchronizer, false, "");
}
TeamBattleCharacter* TeamBattleCharacter::createRemote(Synchronizer *synchronizer,
                                                       string id){
    return create(synchronizer, false, id);
}

TeamBattleCharacter* TeamBattleCharacter::create(Synchronizer *synchronizer,
                                                 bool isLocal, std::string id){
    TeamBattleCharacter *node = new TeamBattleCharacter();
    
    if (node && node->init(synchronizer, isLocal, id))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool TeamBattleCharacter::init(Synchronizer *synchronizer, bool isLocal, std::string id){
    if(!SynchronizedObject::init(isLocal, synchronizer, "TeamBattleCharacter", id)){
        return false;
    }
    _character_status = TeamBattleCharacterStatus::create(100, 50, 100, 1, 1005, TeamBattleCharacterStatus::kFriend, "testUser", "1", "testDeath");
    this->addChild(_character_status);
    _is_fast = true;
    _ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleCharacter::onEnter(){
    SynchronizedObject::onEnter();
    
    initEventListener();
    
    
    createHpGauge();
    createNameLabel();
    createLevelLabel();
    createCardIcon();
    createSoldier();
}


void TeamBattleCharacter::createHpGauge(){
    _hp_gauge_back = Sprite::create("init/Map/gauge_bk.png");
    _hp_gauge_back->setScale(_ratio * 0.7f);
    _hp_gauge_back->setPosition(Vec2(this->getContentSize().width / 2, 98 * _ratio));
    this->addChild(_hp_gauge_back, kZOrderLifeGaugeBack, kTagLifeGaugeBack);
    
    _hp_gauge_bar = Sprite::create("init/Map/gauge_green.png");
    _hp_gauge_bar->setScale(1.0f, _ratio * 0.7f);
    
    _hp_gauge_bar->setAnchorPoint(Vec2::ZERO);
    _hp_gauge_back->addChild(_hp_gauge_bar, kZOrderLifeGaugeBar, kTagLifeGaugeBar);
    _hp_gauge_bar->setPosition(Vec2(12, _hp_gauge_back->getContentSize().height / 2));
}

void TeamBattleCharacter::createNameLabel(){
    _name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_character_status->getUserName(), FONT_NAME_SYSTEM, 22 * _ratio);
    _name_label->setAlignment(TextHAlignment::CENTER);
    _name_label->setPosition(Point(0, 80 * _ratio));
    _name_label->enableOutline(Color4B::BLACK, 4);
    this->addChild(_name_label, kZOrderNameLabel, kTagNameLabel);
}

void TeamBattleCharacter::createLevelLabel(){
    string levelStr = "Lv" + to_string(_character_status->getLevel());
    _level_label = HMSFontUtil::createWithTTFBySystemFontArgument(levelStr, FONT_NAME_SYSTEM, 22 * _ratio);
    _level_label->setAlignment(TextHAlignment::CENTER);
    _level_label->setPosition(Vec2(0, 0 * _ratio));
    _level_label->enableOutline(Color4B::BLACK, 4);
    this->addChild(_level_label, kZOrderLevelLabel, kTagLevelLabel);
}

void TeamBattleCharacter::createCardIcon(){
    string path = "download/Chara/thumb100/" +
                       to_string(_character_status->getCharacterId()) + ".png";
    _card_image = Sprite::create(path.c_str());
    _card_image->setScale(0.7f * _ratio);
    _card_image->setPosition(Point(0, 35 * _ratio));
    this->addChild(_card_image, kZOrderCardImage, kTagCardImage);
    
    //枠画像表示
    path = "init/Map/square_card_blue.png";
    if(_character_status->getTeamType() == TeamBattleCharacterStatus::TeamType::kEnemy) {
        path = "init/Map/square_card_red.png";
    }
    if(isLocal()) {
        path = "init/Map/square_card_green.png";
    }
    _card_back = Sprite::create(path.c_str());
    _card_back->setScale(0.7f * _ratio);
    _card_back->setPosition(Point(0, 35 * _ratio));
    this->addChild(_card_back, kZOrderCardBack, kTagCardBack);

}

void TeamBattleCharacter::createSoldier(){
    string name = "soldier";
    if(_character_status->getTeamType() == TeamBattleCharacterStatus::kEnemy){
        name = "enemy";
    }
    string path = "init/Map/soldier/" + name + "_" + to_string(_direction) + "_1.png";
    _soldier = Sprite::create(path.c_str());
    _soldier->setScale(_ratio);
    _soldier->setPosition(Vec2::ZERO);
    this->addChild(_soldier, kZOrderSoldierImage, kTagSoldierImage);
}

void TeamBattleCharacter::move(float delta){
    if(getPosition().distance(_target_position) < 3){
        setPosition(_target_position);
        return;
    }
    Vec2 diff = _target_position - getPosition();
    diff.normalize();
    Vec2 position = getPosition() + diff * _character_status->getMoveSpeed() * delta;
    setPosition(position);
}








//送信する値を設定
void TeamBattleCharacter::onSend(){
    MultiValue* value = MultiValue::create();
    
    value->push(getPosition());
    
    send(value);
}

//受信した時の挙動を設定
void TeamBattleCharacter::onReceive(MultiValue* value){
    _sync_start_position = getPosition();
    _sync_end_position = PrimitiveParser::asVec2(value->pop().value);
    if(_is_fast) setPosition(_sync_end_position);
    //線形補間用の時間を保存
    _sync_start_time = _elapsed_time;
    _sync_end_time = _elapsed_time + sendCycle();
    
    if(_is_fast) _is_fast = false;
}

//リモートでクローンするときの挙動を設定
SynchronizedObject* TeamBattleCharacter::onClone(MultiValue* value, std::string id,
                                                 Synchronizer* synchronizer){
    TeamBattleCharacter* clone = createRemote(synchronizer, id);
    return clone;
}

//ローカル時の挙動の設定
void TeamBattleCharacter::onLocalUpdate(float delta){
    move(delta);
}

//リモート時の挙動の設定
void TeamBattleCharacter::onRemoteUpdate(float delta){
    if(_is_fast) return;
    
    if(getPosition().distance(_target_position) < 1){
        setPosition(_target_position);
        return;
    }
    
    _elapsed_time += delta;
    float t = (_elapsed_time - _sync_start_time) / (_sync_end_time - _sync_start_time);
    
    Vec2 position = _sync_start_position.lerp(_sync_end_position, t);

    setPosition(position);
}

void TeamBattleCharacter::initEventListener(){
    //タップイベントの設定
    _listener = EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = CC_CALLBACK_2(TeamBattleCharacter::onTouchBegan, this);
    _listener->onTouchMoved = CC_CALLBACK_2(TeamBattleCharacter::onTouchMoved, this);
    _listener->onTouchEnded = CC_CALLBACK_2(TeamBattleCharacter::onTouchEnded, this);
    _listener->onTouchCancelled = CC_CALLBACK_2(TeamBattleCharacter::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
}

bool TeamBattleCharacter::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    if(!isLocal()) return false;
    
    _target_position = touch->getLocation();
    
    return true;
}
void TeamBattleCharacter::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}

void TeamBattleCharacter::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void TeamBattleCharacter::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}
