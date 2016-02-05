#ifndef __war__CharaListSortPopupLayer__
#define __war__CharaListSortPopupLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "SaveData.h"

USING_NS_CC;

class CharaListSortPopupLayer : public Layer
{
private:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title,
        T_All_MenuItem,
        T_Offense_MenuItem,
        T_Defense_MenuItem,
        T_Strategist_MenuItem,
        T_General_MenuItem,
        T_Type_Menu,
        T_Power_MenuItem,
        T_Rare_MenuItem,
        T_NeedCost_MenuItem,
        T_InsertDate_MenuItem,
        T_Sort_Menu,
        T_Close_MenuItem,
        T_Close_Menu,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title,
        Z_All_MenuItem,
        Z_Offense_MenuItem,
        Z_Defense_MenuItem,
        Z_Strategist_MenuItem,
        Z_General_MenuItem,
        Z_Type_Menu,
        Z_Power_MenuItem,
        Z_Rare_MenuItem,
        Z_NeedCost_MenuItem,
        Z_InsertDate_MenuItem,
        Z_Sort_Menu,
        Z_Close_MenuItem,
        Z_Close_Menu,
    };
    
    EventListenerTouchOneByOne *listener;
    
    Sprite* _bg; //背景
    bool _chara_type[4] = {false};
    string _from_page_name; //遷移以前のページ名
    
public:
    //ソート項目
    enum SortStatus{
        Power = 1,
        Rare,
        NeedCost,
        InsertDate,
    };
    
    SortStatus _sort_status;
    
    CharaListSortPopupLayer(); //コンストラクタ
    
    float ratio;
    static CharaListSortPopupLayer* create(bool* chara_type, string from_page_name);
    virtual bool init(bool* chara_type, string from_page_name);
//    CREATE_FUNC(CharaListSortPopupLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void showTItle(); //タイトルの表示
    
    void createCharaTypeButton(); //キャラタイプ絞り込みボタンの作成
    void createSortButton(); //並び替えボタンの作成
    void createChangeButton(); //変更するボタンの作成
    void setSelectButtonData(); //選択されたボタンのデータ設定
    
    void setCharaTypeData(Ref* sender); //キャラタイプ絞り込みのデータの設定
    bool changeBoolean(bool status); //引数のboolと逆のboolを返す関数
    void checkSelected(int tag); //ボタンが押されている状態かチェック
    
    void initCharaTypeData(); //キャラタイプの初期化
    
    void setSortStatusData(Ref* sender); //ソート条件を設定
    void changeButtonAction(); //変更するボタンを押したときの処理
    
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(Touch *touch, Event *unused_event);
    
    void onEnter(); //初期実行関数
};


#endif /* defined(__war__CharaListSortPopupLayer__) */
