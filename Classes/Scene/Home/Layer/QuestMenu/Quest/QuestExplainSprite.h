#ifndef __war__QuestExplainSprite__
#define __war__QuestExplainSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "json11.hpp"

#define EXPLAIN_NEW_TAG 1

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class QuestExplainSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _quest_id, QuestId); //クエストID
    CC_SYNTHESIZE(string, _quest_level, QuestLevel); //クエストレベル
    CC_SYNTHESIZE(string, _quest_name, QuestName); //クエスト名
    CC_SYNTHESIZE(string, _quest_comment, QuestCommnet); //クエスト名
    
    CC_SYNTHESIZE(bool, _new_flg, QuestNewFlg); //クエストNewフラグ
    CC_SYNTHESIZE(int, _achieve_count, QuestAchieveCount); //クエスト達成レベル
    
public:
    QuestExplainSprite(); //コンストラクタ
    
    float ratio;
    
    static QuestExplainSprite* create(Json json);
    virtual bool init(Json json);
    
    void setupData(Json json); //データの設定
    void showQuestName(); //クエスト名の表示
    void showNewSprite(); //New画像の表示
    void showCharaSprite(); //リストのキャラの表示
    void showQuestComment(); //クエストのコメントの表示
    void actionNewSprite(); //New画像のアクション
    void showAchieveStar(); //達成画像の表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__QuestExplainSprite__) */
