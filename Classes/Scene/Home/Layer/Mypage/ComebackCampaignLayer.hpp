#ifndef ComebackCampaignLayer_hpp
#define ComebackCampaignLayer_hpp

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;

class ComebackCampaignLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //オブシェクト作成メソッド
    void createShadow();
    void createPopupImage();
    void createTitleLabel();
    void createButton();
    void createPresentSpriteAndLabel();
    void getImagePathForItemType(string& image_path, string& label_text, Json json);
    void createInfoLabel();
    void createArrowSprite();
    void createSelectedPresentNumberLabel();
    void createDot();
    
    //切り替えメソッド
    void switchDotAndArrowAndNuber(int index);
    void selectedDotItemNumber(int index);
    void checkAndShowArrowSprite(int index);
    void switchPresentNumber(int index);
    
    Node* _node; //この上の画像を載せていきアクションはこれを動かす
    HmsVector<string > _vec_label_text; //アイテム名ベクター
    HmsVector<DrawNode *> _vec_dot;
    HmsVector<Point > _vec_position;
    
    
    Sprite* _popup_bg; //ポップアップ背景
    Sprite* _left_arrow;
    Sprite* _right_arrow;
    
    int _current_index; //現在表示中のインデックス
    int _max_present_count; //最大プレゼント数
    
    Json _comeback_present_data_list;
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title,
        T_Menu,
        T_Clipper,
        T_Present_Sprite,
        T_Present_Label,
        T_Left_Arrow,
        T_Right_Arrow,
        T_Selected_Present_Number,
        T_Dot,
        T_Info_Label,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title,
        Z_Menu,
        Z_Clipper,
        Z_Present_Sprite,
        Z_Present_Label,
        Z_Left_Arrow,
        Z_Right_Arrow,
        Z_Selected_Present_Number,
        Z_Dot,
        Z_Info_Label,
    };
    
    enum SwitchPos{
        Left = 1,
        Right,
    };
    
    float ratio;
    
    ComebackCampaignLayer(); //コンストラクタ
    static ComebackCampaignLayer* create(Json json);
    virtual bool init(Json json);
    
    void initEventListener(); //イベントリスナーの初期化
    
    //アクションメソッド
    void switchSprite(SwitchPos switch_pos);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* ComebackCampaignLayer_hpp */
