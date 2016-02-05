#ifndef _TEAM_MEMBER_LIST_LAYER_H_
#define _TEAM_MEMBER_LIST_LAYER_H_

#include "cocos2d.h"
#include "HmsScrollLayer.h"
#include "HmsList.h"

USING_NS_CC;
using namespace std;

//メンバー一覧
class TeamMemberListLayer : public Layer, public ScrollViewDelegate
{
private:
    
    enum Order{
        O_TAB_BACK,
        O_BACK,
        O_TAB_FRONT,
        O_SCROLLVIEW,
        O_PopupLayer,
    };
    
    enum Tag{
        T_MEMBER=101,
        T_APP,
        T_SOJI,
        T_Search_text,
        T_PopupLayer,
    };
    
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    //用途
    string _use_name;
    
    //チームリストデータ
    Json _json_team_member_list;
    int _count_p;
    int _count_i;
    
    //フラグ
    bool _tap_flg = false;
    
    //スクロール用
    int _cell_count;
    int _cell_size;
    
    //検索用
    CC_SYNTHESIZE(int, sortName, SortName); //名前(new:新着順　login:最終ログイン順　level:Lv順　rank:Rank順)
    CC_SYNTHESIZE(string, searchId, SearchId); //ユーザフレンドID検索
    CC_SYNTHESIZE(string, searchUserName, SearchUserName); //ユーザ名検索
    
public:
    //検索用
    enum SearchTag{
        NewList = 1,
        LoginList,
        LevelList,
        RankList,
    };
    
    virtual bool init();
    CREATE_FUNC(TeamMemberListLayer);

    void onEnter();
    void getData();
    void getDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void createScrollView(); //スクロールビューの作成
    Layer *createList();
    void updateTab();
    void searchInvitationList(); //検索用関数
    void searchButtonCreate(); //検索用ボタン
    void refreshList(); //検索用リスト更新
    string getSearchSortName(int sort_tag);
    string getSearchSortNameDisplay(int sort_tag);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void applyCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void cancelCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void searchInvitationListCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};

#endif