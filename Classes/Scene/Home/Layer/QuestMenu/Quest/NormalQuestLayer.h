#ifndef __War__NormalQuestLayer__
#define __War__NormalQuestLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "QuestSprite.h"

USING_NS_CC;

class NormalQuestLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    
    //タグ
    enum Tag{
        T_Challenge_Sp = 20,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Challenge_Sp = 20,
    };
    
    float ratio;
    Json _quest_data;
    bool challenge_event_flg = false;
    
    HmsVector<Json > _quest_level_data; //レベルソート後のデータ
    HmsVector<Sprite *> _quest_sp;
    
    virtual bool init();
    CREATE_FUNC(NormalQuestLayer);
    
    void initEventListener(); //イベントリスナーの登録
    void createQuestSprite(); //クエストリストの作成
    void changePage(int tag); //ページの遷移
    void toChallengePage(Ref* sender);
    
    void setupLevelData();
    
    //通信処理
    void getQuestDataRequest(); //クエストデータの取得
    void getQuestDataRequestCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__NormalQuestLayer__) */
