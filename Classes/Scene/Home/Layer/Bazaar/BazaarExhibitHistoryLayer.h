#ifndef __war__BazaarExhibitHistoryLayer__
#define __war__BazaarExhibitHistoryLayer__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class BazaarExhibitHistoryLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    json11::Json _history_data;
    
    //タグ
    enum Tag {
        T_Inquiry = 1,
        T_Other,
        T_BG,
        T_Message,
        T_List
    };
    
    enum ZOrder {
        Z_Inquiry = 1,
        Z_Other,
        Z_BG,
        Z_Message,
        Z_List,
        Z_List_base,
        Z_List_Message
    };

public:
    Node* _node; //バザー出品レイヤーのポインタを保持しておく
    
    BazaarExhibitHistoryLayer(); //コンストラクタ
    
    static BazaarExhibitHistoryLayer* create(json11::Json history_data);
    virtual bool init(json11::Json history_data);
    
    float ratio;
    
    void initEventListener(); //イベントリスナーの設定
    string getBackSpritePath(json11::Json json); //背景画像のパスを調べて返す
    void createDisableCascadeNode(); //ノードの作成
    
    void createButton(); //ボタンの作成
    void createBg(); //画面の生成
    void createList(); //リストの生成
    void inPopupAction(); //画面を大きくする
    void outPopupAction(); //画面を消す
    void createListTrue(int loop_count); //本当にリスト生成
    void createListFalse(); //履歴がなかったとき
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__BazaarExhibitHistoryLayer__) */
