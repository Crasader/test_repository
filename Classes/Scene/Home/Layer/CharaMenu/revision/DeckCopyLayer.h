#ifndef __war__DeckCopyLayer__
#define __war__DeckCopyLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"
#include "DeckCopyConfirmLayer.h"

USING_NS_CC;

class DeckCopyLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title,
        T_Close_Menu,
        T_Deck_Menu_1,
        T_Deck_Menu_2,
        T_Deck_Menu_3,
        T_Confirm_Layer,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title,
        Z_Close_Menu,
        Z_Deck_Menu_1,
        Z_Deck_Menu_2,
        Z_Deck_Menu_3,
        Z_Confirm_Layer,
    };
    
    //デッキ番号
    enum DeckNo{
        Deck_1 = 1,
        Deck_2,
        Deck_3,
        Deck_4,
        Deck_5,
        Deck_6,
        Deck_7,
        Deck_8,
        Deck_9,
        Deck_10,
    };
    
    string _deck_no; //現在のデッキ番号
    Sprite* _bg; //背景画像
    
public:
    DeckCopyLayer(); //コンストラクタ
    
    float ratio;
    static DeckCopyLayer* create();
    virtual bool init();
    //    CREATE_FUNC(DeckCopyLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //コピー番号
    void showMessageLabel(); //テキストの表示
    
    void selectedDeckNo(Ref* sender); //デッキ番号選択コールバック
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckCopyLayer__) */
