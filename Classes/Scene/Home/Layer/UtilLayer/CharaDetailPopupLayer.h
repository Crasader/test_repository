#ifndef __war__CharaDetailPopupLayer__
#define __war__CharaDetailPopupLayer__

#include "cocos2d.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "CharaDetailSprite.h"
#include "CharaDetailSpriteList.h"



USING_NS_CC;
using namespace std;

class CharaDetailPopupLayer: public Layer
{
private:
    
    const int TAG_SHADOW = 99 ;
    
    EventListenerTouchOneByOne *listener;
    void initEventListener();
    void createDetailSprite();
    void createDetailSprites();
    void createShadowSprite();
    virtual bool init(int chara_list_id, bool auto_remove, bool hide_lock_button);
    float ratio;
    
    int _chara_list_id ;
    HmsVector<int> _chara_list_id_list;
    
    //窓を閉じるときに呼び出し元から登録されたメソッドを呼び出すために保持する変数
    Layer *_parent;
    SEL_SCHEDULE _sel_for_close;
    
    //base_indexが変更されたときに呼び出されるメソッドを保持
    SEL_SCHEDULE _sel_for_change_base;
    
    CharaDetailSprite* _chara_detail_sprite ;
    CharaDetailSpriteList *_chara_detail_sprite_list;
    
    bool _auto_remove;
    bool _hide_lock_button;
    
public:
    void hideLockButton();
    
    bool isLockStateChanged() ;
    bool getLockFlg() ;
    int getBaseIndex() ;
    
    //窓を閉じるときに呼び出すメソッドを登録する
    void setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel) ;
    
    //base_indexが変更されたとき呼び出すメソッドを登録する
    void setSchedulerForChangeBase(Layer *parent, SEL_SCHEDULE sel);
    
    void onEnter() ;
    
    //auto_removeをfalseにした場合、クローズボタンが押されたときに自動で自分自身を削除する。
    //自動削除しない場合はコールバックから削除する必要がある
    static CharaDetailPopupLayer* create(int chara_list_id, bool auto_remove, bool hide_lock_button);
    static CharaDetailPopupLayer* create(int chara_list_id, bool auto_remove);
    static CharaDetailPopupLayer* create(int chara_list_id);
    void setCharaListIdList(HmsVector<int> chara_list_id_list);
    void removeSelf(float dt);
    //CharaDetailSpriteのクローズボタンが押されたときに呼び出されるコールバック
    void notifyClose(float dt);
    void notifyChangeBaseIndex(float dt) ;
};


#endif /* defined(__war__CharaDetailPopupLayer__) */


