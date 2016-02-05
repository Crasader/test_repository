#ifndef __war__CharaEvolutionConfirmLayer__
#define __war__CharaEvolutionConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CardDetailLayer.h"

USING_NS_CC;

class CharaEvolutionConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _chara_id;
    Json _chara_data_list; //キャラデータリスト
    HmsVector<CharaSprite *> _chara; //キャラ画像
    CharaSprite * _next_chara;
    string _next_chara_id;
    string _next_chara_list_id;
    string _next_chara_active_skill_name;
    string _next_chara_active_skill_cost;
    string _next_chara_passive_skill_name;
    string _pre_chara_active_skill_name;
    string _pre_chara_active_skill_cost;
    string _pre_chara_passive_skill_name;
    
    bool enable_evolution_flg = false;
    
    int hit_card[30];          //引いたカードのID
    int rare_card[30];         //引いたカードのレアリティ
    int chara_list_id[30];     //引いたカードのcard_list_id
    int chara_type[30];        //引いたカードのchara_type
    string card_name_list[30]; //引いたカードの名前
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CharaEvolutionConfirmLayer);
    
    void initEventListener(); //イベントリスナーの設定
    
    //キャラデータ通信処理
    void getCharaDataRequest();
    void charaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //セットアップ
    void setup();
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    //キャラデータ通信処理
    void doEvolutionRequest();
    void doEvolutionCallback(HttpClient *sender, HttpResponse *response);
    
    void screenTransition();
};

#endif /* defined(__war__CharaEvolutionConfirmLayer__) */
