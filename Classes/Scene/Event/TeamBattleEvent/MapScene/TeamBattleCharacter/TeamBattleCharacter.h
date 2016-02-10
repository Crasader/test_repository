#ifndef __war__TeamBattleCharacter__
#define __war__TeamBattleCharacter__

#include "cocos2d.h"
#include "SynchronizedObject.h"

USING_NS_CC;

class TeamBattleCharacterStatus;

class TeamBattleCharacter : public SynchronizedObject{
public:
    static TeamBattleCharacter* createLocal(Synchronizer* synchronizer);
    static TeamBattleCharacter* createRemote(Synchronizer* synchronizer, std::string id);
    static TeamBattleCharacter* createPrototype(Synchronizer* synchronizer);
    
    static TeamBattleCharacter* create(Synchronizer* synchronizer,
                                       bool isLocal, std::string id);
    virtual bool init(Synchronizer* synchronizer,
                      bool isLocal, std::string id);
    
private:
    enum Tag{
        kTagSoldierImage = 0,
        kTagLifeGaugeBack,
        kTagLifeGaugeBar,
        kTagCardBack,
        kTagCardImage,
        kTagNameLabel,
        kTagLevelLabel,
    };
    enum ZOrder{
        kZOrderSoldierImage = 0,
        kZOrderLifeGaugeBack,
        kZOrderLifeGaugeBar,
        kZOrderCardBack,
        kZOrderCardImage,
        kZOrderNameLabel,
        kZOrderLevelLabel,
    };
    
    void createHpGauge();
    void createNameLabel();
    void createLevelLabel();
    void createCardIcon();
    void createSoldier();
    
    void updateSoldierSprite();
    void updateHpGauge(float delta);
    void move(float delta);
    
    virtual void onSend();
    virtual void onReceive(MultiValue* value);
    virtual SynchronizedObject* onClone(MultiValue* value, std::string id,
                                        Synchronizer* synchronizer);
    //ローカル側の更新処理
    virtual void onLocalUpdate(float delta);
    //リモート側の更新処理
    virtual void onRemoteUpdate(float delta);
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    Sprite* _hp_gauge_back;
    Sprite* _hp_gauge_bar;
    Label* _name_label;
    Label* _level_label;
    Sprite* _card_image;
    Sprite* _card_back;
    Sprite* _soldier;
    
    float _ratio;
    EventListenerTouchOneByOne* _listener;
    TeamBattleCharacterStatus* _character_status;
    float _elapsed_time;
    Vec2 _sync_start_position;
    Vec2 _sync_end_position;
    float _sync_start_time;
    float _sync_end_time;
    bool _is_fast;

    int _direction;
    Vec2 _target_position;
    
};


#endif /* defined(__war__TeamBattleCharacter__) */
