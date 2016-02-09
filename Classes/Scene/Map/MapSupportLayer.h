#ifndef __War__MapSupportLayer__
#define __War__MapSupportLayer__

#include "cocos2d.h"
#include "HmsVector.h"
#include "MapCharaLayer.h"
#include "HttpConnect.h"

USING_NS_CC;

class MapSupportLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    
    int battle_list_id;
    Sprite* _bg; //背景画像
    bool _shadow_flg; //影をつけるかのフラグ
    
    string _boss_event_stage_log_id;
    HmsVector<MapCharaLayer *> _chara_list;
    int _target_user_master_id;
    int _now_support_user_master_id;
public:
    float ratio;
    
    static MapSupportLayer* create(string message, bool shadow_flg, HmsVector<MapCharaLayer *> chara_list, string boss_event_stage_log_id, int now_support_user_master_id);
    virtual bool init(string message, bool shadow_flg, HmsVector<MapCharaLayer *> chara_list, string boss_event_stage_log_id, int now_support_user_master_id);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void showMessage(); //テキストの表示
    void createList();
    Sprite *getBackground() ; //ポップアップ背景の取得
    
    void sendCheerRequest();
    void sendCheerRequestCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__MapSupportLayer__) */
