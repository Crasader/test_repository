#ifndef __War__ActiveSkillSprite__
#define __War__ActiveSkillSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace json11;

class ActiveSkillSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _active_skill_id, ActiveSkillId);//アクティブスキルID
    CC_SYNTHESIZE(string, _active_skill_name, ActiveSkillName);//アクティブスキル名
    CC_SYNTHESIZE(string, _active_skill_need_cost, ActiveSkillNeedCost);//アクティブスキル発動コスト
    CC_SYNTHESIZE(string, _active_skill_detail, ActiveSkillDetail);//アクティブスキルの説明
public:
    ActiveSkillSprite(); //コンストラクタ
    float ratio;
    
    static ActiveSkillSprite* create();
    virtual bool init();
    
    void setupSkillData(Json json); //スキルデータの取得
    void showSkillData(); //ユーザデータの表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__ActiveSkillSprite__) */
