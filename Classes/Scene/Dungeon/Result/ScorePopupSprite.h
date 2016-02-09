#ifndef __War__ScorePopupSprite__
#define __War__ScorePopupSprite__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "HttpConnect.h"
#include "json11.hpp"

#define ITEM_1_POS_X 120
#define ITEM_1_POS_Y 295
#define ITEM_DISTANCE_X 260
#define ITEM_DISTANCE_Y 150

#define ITEM_BOX_WIDTH 510
#define ITEM_BOX_HEIGHT 350

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class ScorePopupSprite : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    Json json;
    int _score;
    bool _created_next_dungeon ; //次のDungeonScene生成済み
    
    string ranking_event_stage_id;
    string choice_friend_id;
    bool get_score_flg = false;
    HmsVector<Layer *> layer_list;
    int layer_index = 0;
    
    bool _now_move_layer ;
public:
    float ratio;
    
    ScorePopupSprite(); //コンストラクタ
    
    static ScorePopupSprite* create(Json json);
    virtual bool init(Json json);
    
    //トータルスコア取得
    void getTotalScore();
    void getTotalScoreCallback(HttpClient *sender, HttpResponse *response);
    
    //レイヤー移動
    void moveLayer();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    //プレゼント数の更新
    void updatePresentCount() ;
};

#endif /* defined(__War__ScorePopupSprite__) */
