#ifndef __war__GooglePlayReviewLayer__
#define __war__GooglePlayReviewLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

/*
#if IF_IOS
このファイルはiosのビルド対象から外してください
#endif
 */

class GooglePlayReviewLayer : public cocos2d::Layer
{
private:
    string _user_master_id; //ユーザーマスターID
    
    GooglePlayReviewLayer(); //コンストラクタ
    
    enum Tag{
        T_Popup = 1,
    };
    
    enum Order{
        O_Popup = 1,
        O_Star,
        O_Label,
    };
    
    enum State{
        ASC_STARS, //星の個数を選んでもらう
        GOOGLE_REVIEW, //googleレビューに遷移させる画面
        HMS_REVIEW, //自社のレビューに遷移させる
        INVALID_REVIEW=100,
    };
    
    State _state ;
    
    EventListenerTouchOneByOne *listener;
    
    //左のボタンが押された
    void onLeftButton(Ref* sender) ;
    //右のボタンが押された
    void onRightButton(Ref* sender) ;
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    Sprite* _bg; //背景画像
    
    //星の画像の作成
    HmsVector<Sprite *> _stars;
    int _touch_begin_star_index ; //タップし始めの星のindex
    bool _enable_touch_flg  ; //タップ不可能にするフラグ
    void onTouchStar(int index);
    
    //ユーザーがレビューを拒否
    void userRefuseReviewRequest() ;
    void userRefuseReviewCallback(HttpClient *sender, HttpResponse *response);
    
    //ユーザーが評価した星の数を送信
    void userReviewResultRequest(int review_point, int next_ask_day) ;
    void userReviewResultCallback(HttpClient *sender, HttpResponse *response);
    
public:
    float ratio;
    
    static GooglePlayReviewLayer* create();
    virtual bool init();
    
    void initEventListener(); //イベントリスナーの登録
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void createLabel() ; //ラベルの作成
    void updateLabel() ; //ラベルの更新
    void createStars(); //星の生成
    
    Label *_left_button_label ;
    Label *_right_button_label ;
    Label *_title_label ;
    Label *_comment_label ;
    
    void setIdForBattleRestart(int battle_list_id);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__GooglePlayReviewLayer__) */

