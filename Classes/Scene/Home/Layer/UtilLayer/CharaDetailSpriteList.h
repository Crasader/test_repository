#ifndef __war__CharaDetailSpriteList__
#define __war__CharaDetailSpriteList__

/*
 このスプライトはCharaDetailPopupLayerを介して使用して下さい
 */

#include "cocos2d.h"
#include "common.h"

#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "cocos-ext.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "CharaDetailSprite.h"
#include "network/HttpClient.h"
#include "json11.hpp"

USING_NS_CC;
    
#define HTTP_SKILL_DATA_URL "chara/get_skill_data.php" //スキルデータ取得PHP

using namespace std;
using namespace json11;
using namespace CocosDenshion;

class CharaDetailSpriteList : public Layer
{
private:
    HmsVector<int> _chara_list_id_list;//create時の引数受け取り
    int _chara_list_id;
    EventListenerTouchOneByOne *listener;
    Point _touchPoint;
    
    bool _hide_lock_button;
    
    //コールバックを通知されるインスタンス
    Layer *_parent;
    
    //窓を閉じるときに通知を行うための変数
    SEL_SCHEDULE _sel_for_close;
    
    //カード詳細を左右に切り替えたときに通知を行うための変数
    SEL_SCHEDULE _sel_for_change;
    
    //レイヤー移動中かどうか
    bool move_flg = false;
    
    //移動距離の倍率
    float swipe_ratio = 1.2f;
    
    int max_x;//レイヤーの最大X座標
    int min_x;//レイヤーの最小X座標
    float base_y;
    float scroll_width;//スクロール幅
    int base_index;//基準のindex
public:
    static CharaDetailSpriteList* create(HmsVector<int> chara_list_id_list, int chara_list_id, bool hide_lock_button);
    virtual bool init(HmsVector<int> chara_list_id_list, int chara_list_id, bool hide_lock_button);
    
    void notifyClose(float dt);
    void notifyChangeBaseIndex(float dt);
    
    //窓を閉じるときに呼び出すメソッドを登録する
    void setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel) ;
    
    //base_indexが変更された時に呼び出すメソッドを登録する
    void setSchedulerForChangeBaseIndex(Layer *parent, SEL_SCHEDULE sel) ;
    
    //base_indexの取得
    int getBaseIndex() ;
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
};

#endif /* defined(__war__CharaDetailSpriteList__) */
