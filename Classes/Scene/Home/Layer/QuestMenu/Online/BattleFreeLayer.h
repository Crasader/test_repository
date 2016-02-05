#ifndef __War__BattleFreeLayer__
#define __War__BattleFreeLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserInfoCellSprite.h"
#include "HmsList.h"
#include "HomeCommon.h"

#define MESSEGE_BAR_TAG 1

USING_NS_CC;

#define IMG_OK_BT "init/Home/Common/bt_ok.png"                  //OKボタン画像

class BattleFreeLayer : public Layer
{
private:
    enum BattleFreeLayerTag{
        Tag_list_start= 0,
        Tag_list_end = 4
    };
    enum BattleFreeLayerZOrder {
        ZOrder_button = 1,
        ZOrder_popup,
    };
    HmsVector<UserInfoCellSprite *> userInfoCells; //ユーザー情報のセルを格納
    HmsVector<int> battle_list_id_list;
    
    void initBattleUserSprite(Json json, int count) ;
    void createList() ;
    Json jsonParseForUserInfoCellSprite(Json json, int count) ;
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(BattleFreeLayer);
    
    void changePage(Ref* sender);
    
    //初期実行関数
    virtual void onEnter();
        
    //通信処理
    void battleUserRequest();//対戦相手取得リクエスト
    void battleUserRequestCallback(HttpClient *sender, HttpResponse *response);//対戦相手選出レスポンス
};

#endif /* defined(__War__BattleFreeLayer__) */
