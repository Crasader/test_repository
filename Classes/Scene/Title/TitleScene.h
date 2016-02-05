#ifndef __TitleScene__
#define __TitleScene__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "NormalScene.h"
#include "DownloadScene.h"
#include "TutorialScene.h"
#include "TutorialBattleScene.h"
#include "TouchScreenSprite.h"
#include "LoginBounsPopupLayer.h"
#include "HmsConsoleUtil.h"
#include "PopupModelChangeLayer.h"
#include "HmsUtilityFunction.h"
#include "NativeBridge.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace network;
using namespace json11;

class TitleScene : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _user_master_id; //ユーザーマスターID
    string _password; //パスワード
    
    Json _online_battle_list; //オンラインのバトルリスト
    Json _restart_data_list; //再開データリスト
    Json _tournament_data_list; //トーナメントデータリスト
    TouchScreenSprite* _touch_sp; //タッチスクリーン画像
    
    bool _needs_download ; //ダウンロードが必要か
    string _resource_version; //
    
public:
    
    TitleScene(); //コンストラクタ
    
    //タグ
    enum Tag{
        T_Bg = 1,
        T_Popup_Layer,
        T_Info_Menu,
        T_Info_MenuItem,
        T_ModelChange_MenuItem,
        T_Inquiry_MenuItem,
        T_Inquiry_Menu,
        T_WebView_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Bg = 1,
        Z_Inquiry_Menu,
        Z_Popup_Layer,
        Z_Info_MenuItem,
        Z_Info_Menu,
        Z_Inquiry_MenuItem,
        Z_WebView_Layer,
        Z_Takeover_Popup,
    };
    
    float ratio;
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(TitleScene);
    
    void createBg(); //背景画像の作成
    void cutLeftAndRightVisibleSpace() ; //両サイドの余白に画像を配置する
    void createVersionCode() ; //バージョン番号の作成
    void setupBgm(); //BGMの作成
    void checkUserData(); //ユーザー登録されているかチェック
    void touchStart(); //タッチスタートアクションの開始
    void createTutorialButton(); //デバック用チュートリアルボタン
    
    void changeScene(); //シーンの切り替え遷移
    void cancelBattleCallback(HttpClient *sender, HttpResponse *response);
    void changeTutorialScene(); //チュートリアルへの遷移
    void loginBounsCheck(); //ログインボーナスチェック
    void createInfoButton(); //お知らせ、問い合わせボタンの作成
    
    //通信処理
    void userRegistRequest();
    void userRegistCallback(HttpClient *sender, HttpResponse *response);
    void userCheckRequest();
    void userCheckCallback(HttpClient *sender, HttpResponse *response);

    //スターティングデータ処理
    void getStartingDataRequest();
    void startingDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベントON OFF切り替え
    void touchON();
    void touchOFF();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //素材ダウンロード関係
    void setupResource();
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__Chrono_Saga_v3b__TitleScene__) */
