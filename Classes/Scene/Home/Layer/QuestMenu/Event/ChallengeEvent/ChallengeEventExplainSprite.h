#ifndef __war__ChallengeEventExplainSprite__
#define __war__ChallengeEventExplainSprite__

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

class ChallengeEventExplainSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _event_id, ChallengeEventId); //イベントID
    CC_SYNTHESIZE(string, _event_name, ChallengeEventName); //イベント名
    CC_SYNTHESIZE(string, _event_comment, ChallengeEventCommnet); //イベント説明名
    
public:
    ChallengeEventExplainSprite(); //コンストラクタ
    
    float ratio;
    
    static ChallengeEventExplainSprite* create(Json json, string image_path);
    virtual bool init(Json json, string image_path);
    
    void setupData(Json json); //データの設定
    void showChallengeEventComment(); //イベントのコメントの表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ChallengeEventExplainSprite__) */
