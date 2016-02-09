#ifndef __war__EventExplainSprite__
#define __war__EventExplainSprite__

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

class EventExplainSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _event_id, EventId); //イベントID
    CC_SYNTHESIZE(string, _event_name, EventName); //イベント名
    CC_SYNTHESIZE(string, _event_comment, EventCommnet); //イベント説明名
    CC_SYNTHESIZE(int, _achieve_count, EventAchieveCount); //イベント達成レベル
    
public:
    EventExplainSprite(); //コンストラクタ
    
    float ratio;
    
    static EventExplainSprite* create(Json json, string image_path);
    virtual bool init(Json json, string image_path);
    
    void setupData(Json json); //データの設定
    void showEventComment(); //イベントのコメントの表示
    void showAchieveStar(); //達成画像の表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__EventExplainSprite__) */
