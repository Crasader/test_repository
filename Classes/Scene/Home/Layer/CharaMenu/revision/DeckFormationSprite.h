#ifndef __war__DeckFormationSprite__
#define __war__DeckFormationSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "CharaSprite.h"
#include "json11.hpp"

#define DECK_1_X_POS 90
#define DECK_1_Y_POS 380
#define DECK_1_X_DISTANCE 107
#define DECK_1_Y_DISTANCE 102

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class DeckFormationSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _general_card_id, GeneralCardId); //総大将カードID
    CC_SYNTHESIZE(string, _deck_name, DeckName); //デッキ名
    CC_SYNTHESIZE(string, _offense, Offense); //攻撃
    CC_SYNTHESIZE(string, _defense, Defense); //防御
    CC_SYNTHESIZE(string, _strategist, Strategist); //軍師
    CC_SYNTHESIZE(string, _general, General); //大将
    
    int _deck_no; //デッキナンバー
    Json _chara_data_list; //キャラデータリスト
    Json _user_party_list; //パーティーリスト
    
    HmsVector<Json > _vec_chara_list; //キャラデータリスト
    
public:
    
    //タグ
    enum Tag{
        T_Deck_Number = 1,
        T_Deck_Name,
        T_Card = 100,
        T_Ban_Sprite = 200,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Deck_Number = 1,
        Z_Deck_Name,
        Z_Card = 100,
        Z_Ban_Sprite = 200,
        
    };
    
    float ratio;
    DeckFormationSprite(); //コンストラクタ
    
    static DeckFormationSprite* create(HmsVector<Json> &vec_chara_data, int deck_no);
    virtual bool init(HmsVector<Json> &vec_chara_data, int deck_no);
    
    void setupData(); //ユーザー情報の設定
    void showDeckName(); //デッキ名の表示
    void showGeneralChara(); //総大将カードの作成表示
    void showDeckParam(); //デッキの数値を表示
    void generateCharaCard(); //背景画像の生成
    Point calcCardPosition(int i, int j); //カードの位置を計算
    
    void refreshDeckSprite(HmsVector<Json > &vec_chara_data);
    
    void onEnter(); //初期実行関数
};


#endif /* defined(__war__DeckFormationSprite__) */
