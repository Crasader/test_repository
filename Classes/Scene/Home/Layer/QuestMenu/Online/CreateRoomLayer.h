#ifndef __War__CreateRoomLayer__
#define __War__CreateRoomLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "GpsManager.h"

USING_NS_CC;

class CreateRoomLayer : public Layer, public GpsDelegate
{
private:
    Sprite* _bg; //背景画像
    Label* _display_num; //表示番号
    string _room_key; //ルームキー
    string _gps_data = "";
    
    Json _battle_room_list; //ルームリスト
    MenuItemSprite* _cancel_btn; //キャンセルボタン
    MenuItemSprite* _confirm_btn; //最終確認ボタン
    
    Json json;

public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CreateRoomLayer);

    void createBg(); //背景の作成
    void createButton(); //キャンセルボタンと対戦確認画面へのボタン
    void setDungeonData(); //ダンジョンシーンのデータの設定
    
    //ルーム作成通信処理
    void createRoomCallback(HttpClient *sender, HttpResponse *response);
    
    //対戦中の履歴をキャンセルする通信処理
    void cancelBattleRequest();
    void cancelBattleCallback(HttpClient *sender, HttpResponse *response);
    
    //マッチング中待機通信処理
    void waittingBattleRequest();
    void waittingBattleCallback(HttpClient *sender, HttpResponse *response);
    void cancelRoomCallback(HttpClient *sender, HttpResponse *response);
    
    //GPSデータ処理
    virtual void onSuccess(std::string gps_data);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__CreateRoomLayer__) */
