#ifndef __War__BattleTopLayer__
#define __War__BattleTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UIAsyncImageSprite.h"
#include "HomeCommon.h"

#define QUICK_BUTTON_TAG 1
#define ROOM_BUTTON_TAG 2
#define REPLAY_BUTTON_TAG 3

USING_NS_CC;

class BattleTopLayer : public Layer
{
private:
//    void initUseDungeonData() ; //ダンジョンシーンで使用するデータを初期化
    void initBattleUserSpriteData() ; //デッキ選択画面で使用するデータの初期化
	void ArenaChallengerCheckRequest(); //クイック対戦に対戦相手が存在するかチェック。
	void ArenaChallengerCheckCallback(HttpClient *sender, HttpResponse *response);//クイック対戦に対戦相手が存在するかチェック(コールバック)
    
public:
    float ratio;
	string _challenger_exists_result; //クイック対戦に対戦相手が存在するかチェックした結果(いる:challenger_coming いない:challenger_nothing)
	string _rank_point_up_rate; //オンライン対戦時のポイント倍率
    virtual bool init();
    CREATE_FUNC(BattleTopLayer);
    
    void createButton(); //ボタンの作成
    void changePage(Ref* sender);
    
    //初期実行関数
    virtual void onEnter();
};

#endif /* defined(__War__BattleTopLayer__) */
