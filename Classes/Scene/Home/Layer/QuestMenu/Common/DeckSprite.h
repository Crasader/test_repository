#ifndef __War__DeckSprite__
#define __War__DeckSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "json11.hpp"

//#define POS_X_1 110
//#define POS_Y_1 164
#define SQUARE_LEFT_X 45
#define SQUARE_BOTTOM_Y 20

#define HALF_SQUARE_HEIGHT 196
#define SQUARE_WIDTH 516

#define INNER_SQUARE_CARD_POS_X_1 70
#define INNER_SQUARE_CARD_POS_Y_1 340

#define DISTANCE_X 96
#define DISTANCE_Y 96


#define DECK_CARD_NUM 20

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class DeckSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _general_card_id, GeneralCardId); //総大将カードID
    CC_SYNTHESIZE(string, _friend_card_id, FriendCardId); //フレンドカードID
    CC_SYNTHESIZE(string, _deck_name, DeckName); //デッキ名
    CC_SYNTHESIZE(string, _offense, Offense); //攻撃
    CC_SYNTHESIZE(string, _defense, Defense); //防御
    CC_SYNTHESIZE(string, _strategist, Strategist); //軍師
    CC_SYNTHESIZE(string, _general, General); //大将
    CC_SYNTHESIZE(string , _json, Json); //Jsonの情報を保持
    
    Sprite* _general_card; //総大将の画像
    
    vector<Sprite *> _deck_card; //デッキのカード
    
    int _deck_no; //デッキの番号
    Json _user_party_list; //ユーザーのデッキリスト
    
    //デッキ表示部分関係
    ClippingNode *createClippingNode() ;
    Sprite* _deck_base_square ;
    Sprite* _scroll_bar;
    int deck_position_state ; // 0:上部分表示 1:下部分表示
    void createScrollBar() ;
    
    
public:
    //10枚のデッキの表示をスクロールして切り替える
    void scrollAction(float dt) ;
    
    float ratio;
    DeckSprite(); //コンストラクタ
    
    static DeckSprite* create(int deck_no);
    virtual bool init(int deck_no);
    
    void setupData(); //ユーザー情報の設定
    void showData(); //ユーザー情報の表示
    void showDeckParam(); //デッキパラメータの表示
    void showDeckName(); //デッキ名の表示

    void showGeneralCard(); //総大将カードの表示
    void showFriendCard(); //フレンドカードの表示
    
    void createDeckCard(); //デッキカードの作成
    void showDeckCard(); //デッキカードの表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__DeckSprite__) */
