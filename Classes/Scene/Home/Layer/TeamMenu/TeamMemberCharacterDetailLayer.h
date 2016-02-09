#ifndef _TEAM_MEMBER_CHARACTER_DETAIL_LAYER_H_
#define _TEAM_MEMBER_CHARACTER_DETAIL_LAYER_H_

#include "cocos2d.h"
#include "common.h"
//#include "HmsLayer.h"
#include "HmsList.h"

USING_NS_CC;
using namespace std;

//フレンドキャラ詳細
class TeamMemberCharacterDetailLayer : public Layer
{
protected:
    
    //用途
    string use_name;
    
    HmsList list;
    
    //選択メンバーインデックス
    int member_index;
    
    //選択中パーティID
    int select_party_id;
    
    //選択タブ
    string tab_name;
    
public:
    
public:
    
    //初期処理
    virtual bool init();
    
    //セット
    void setData(int user_master_id, int select_party_id, string use_name);
    
    //更新
    void update();
    
    //終了前フェードアウト
    void finishFadeOut();
    
    //終了
    void finish();
    
    CREATE_FUNC(TeamMemberCharacterDetailLayer);
    
    //タップイベント
    //virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    //virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    //virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif