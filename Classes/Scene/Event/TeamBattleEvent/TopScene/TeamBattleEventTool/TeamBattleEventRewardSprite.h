#ifndef __war__TeamBattleEventRewardSprite__
#define __war__TeamBattleEventRewardSprite__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "Utility.h"
#include "json11.hpp"
#include "HmsTemplateFunction.h"

USING_NS_CC;

class TeamBattleEventRewardSprite : public Sprite
{
public:
    //画像タイプ
    enum ResultImageType{
       WIN_PLACE, //勝利チーム
       LOSE_PLACE, //敗北チーム
    };
    
    ResultImageType _result_image_type;
    
    TeamBattleEventRewardSprite();
    
    static TeamBattleEventRewardSprite* create(json11::Json json);
    virtual bool init(json11::Json json);
    
    void onEnter(); //初期実行関数
    
    
private:
    
    CC_SYNTHESIZE(string, _result_type, ResultType);
    vector<json11::Json> _reward_data;
    
//    CC_SYNTHESIZE(string, _count, Count);
//    CC_SYNTHESIZE(string, _reward_name, RewardName);
//    CC_SYNTHESIZE(string, _item_type, ItemType);
//    CC_SYNTHESIZE(string, _related_primary_id, RelatedPrimaryId);
//    CC_SYNTHESIZE(string, _count, Count);
    
    //タグ
    enum Tag{
        T_SOMETHING1,
        T_SOMETHING2,
    };
    
    //ZOrder
    enum ZOrder{
        Z_SOMETHING1,
        Z_SOMETHING2,
    };
    
    void setupData(json11::Json json); //データの設定
    
    void createRewardImage(); //報酬画像の作成
    void createRewardList(); //報酬一覧
    
};

#endif /* defined(__war__TeamBattleEventRewardSprite__) */
