#ifndef __war__RankingEventExplainSprite__
#define __war__RankingEventExplainSprite__

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

class RankingEventExplainSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _event_comment, EventComment); //イベント説明名
    
public:
    RankingEventExplainSprite(); //コンストラクタ
    
    float ratio;
    
    static RankingEventExplainSprite* create(string comment, string image_path);
    virtual bool init(string comment, string image_path);
    
    void showEventComment(); //イベントのコメントの表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__RankingEventExplainSprite__) */
