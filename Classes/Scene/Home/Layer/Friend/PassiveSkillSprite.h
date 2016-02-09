#ifndef __War__PassiveSkillSprite__
#define __War__PassiveSkillSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"
//#include "NormalScene.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class PassiveSkillSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _passive_skill_id, PassiveSkillId);//パッシブスキルID
    CC_SYNTHESIZE(string, _passive_skill_name, PassiveSkillName);//パッシブスキル名
    CC_SYNTHESIZE(string, _passive_skill_detail, PassiveSkillDetail);//パッシブスキルの説明
public:
    PassiveSkillSprite(); //コンストラクタ
    float ratio;
    
    static PassiveSkillSprite* create();
    virtual bool init();
    
    void setupSkillData(Json json); //スキルデータの取得
    void showSkillData(); //スキルデータの表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PassiveSkillSprite__) */
