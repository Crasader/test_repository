#ifndef __war__TournamentRecordLayer__
#define __war__TournamentRecordLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "ReplayListSprite.h"

USING_NS_CC;

class TournamentRecordLayer : public Layer
{
private:
    
    EventListenerTouchOneByOne *listener;
    Json _record_data;
    Json _replay_data_list;
    
    HmsVector<string> _vec_record_data;
    
public:
    
    enum Tag{
        T_Bg_Record = 1,
        T_Bg_Replay,
        T_Record_Label,
        T_Replay_Sprite,
    };
    
    enum ZOrder{
        Z_Bg_Record = 1,
        Z_Bg_Replay,
        Z_Record_Label,
        Z_Replay_Sprite,
    };
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TournamentRecordLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createRecordLabelList();
    
    //データ通信処理
    void getRecordDataRequest();
    void recordDataCallback(HttpClient *sender, HttpResponse *response);
    void createDataList();
    
    void showReplayList();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentRecordLayer__) */
