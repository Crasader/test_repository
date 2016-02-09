#ifndef __War__SearchRoomLayer__
#define __War__SearchRoomLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "GpsManager.h"

#define MAX_INSERT_NUM 5
#define BUTTON_NUM 10

USING_NS_CC;

class SearchRoomLayer : public Layer, public GpsDelegate
{
private:
    HmsVector<MenuItemSprite *> _menu_item; //数字メニューボタン
    MenuItemSprite *ok_button;
    Sprite* _bg; //背景画像
    Label* _display_num; //表示番号
    Json json;
    string _gps_data = "";
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(SearchRoomLayer);
    
    void createBg(); //背景の作成
    void createNumButton(); //数字ボタンの作成
    void createButton(); //クリア、OKボタン
    
    //void pushNumberButton(Ref* sender); //数字ボタンを押したとき
    //void changeNumLabel(int number); //押された番号を表示する
    //void clearNum(); //入力数字をクリアする
    void setDungeonData(); //ダンジョンシーン用のデータの設定
    
    //対戦相手（ルーム）検索通信
    void searchRoomRequest();
    void searchRoomCallback(HttpClient *sender, HttpResponse *response);
    
    //GPSデータ処理
    virtual void onSuccess(std::string gps_data);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__SearchRoomLayer__) */
