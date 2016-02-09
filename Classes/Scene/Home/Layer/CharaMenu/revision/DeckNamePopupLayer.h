#ifndef __war__DeckNamePopupLayer__
#define __war__DeckNamePopupLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"
//#include "ui/CocosGUI.h"

USING_NS_CC;

class DeckNamePopupLayer : public Layer, public ui::EditBoxDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
    };
    
    Sprite* _bg; //背景画像
    int _deck_no; //デッキ番号
    string _deck_name; //デッキ名
    
public:
    DeckNamePopupLayer(); //コンストラクタ
    
    float ratio;
    static DeckNamePopupLayer* create(int deck_no, string deck_name);
    virtual bool init(int deck_no, string deck_name);
    //    CREATE_FUNC(DeckNamePopupLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTextField(); //デッキ名を入力するテキストフィールドの作成
    void createButton(); //はい、いいえボタンの作成
    
    void selectedDeckType(Ref* sender);
    
//    void textFieldEvent(Ref* sender, cocos2d::ui::TextField::EventType type);
    
    //デッキ名変更通信処理
    void changeDeckNameDataRequest();
    void changeDeckNameDataCallback(HttpClient *sender, HttpResponse *response);
    
    /* EditBox処理 */
    virtual void editBoxEditingDidBegin(ui::EditBox *editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox *editBox);
    virtual void editBoxTextChanged(ui::EditBox *editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox *editBox);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckNamePopupLayer__) */
