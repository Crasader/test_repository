#ifndef __War__LoginBounsPopupLayer__
#define __War__LoginBounsPopupLayer__

#include "cocos2d.h"
#include "TitleScene.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;

class LoginBounsPopupLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    Json _login_present_list; //ログインプレゼントリスト
    
    Sprite* _bg; //背景画像
    MenuItemSprite* _ok_btn; //OKボタン
    Sprite* _present_sp; //プレゼント画像
    Label* _present_name_label; //プレゼント名
    
    vector<Json > _present_data; //プレゼントデータ
    
    CC_SYNTHESIZE(string, _login_campaign_title, LoginCampaignTitle); //キャンペーン名
    CC_SYNTHESIZE(string, _item_type, ItemType); //プレゼントの種類
    CC_SYNTHESIZE(string, _chara_name, CharaName); //キャラ名
    CC_SYNTHESIZE(string, _related_primary_id, Related_primary_id); //キャラID
    CC_SYNTHESIZE(int, _present_count, PresentCount); //プレゼント数
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ閉じるアクション
    
    int _loop_count; //ポップアップ表示回数
    
public:
    float ratio;
    
    LoginBounsPopupLayer(); //コンストラクタ
    static LoginBounsPopupLayer* create(Json json);
    virtual bool init(Json json);
    
    void parsePresentList(); //プレゼントリストの分解
    void setPresentData(); //プレゼントデータの設定
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void showCampaignTitle(); //キャンペーン名の表示
    void showMessage(); //テキストの表示
    void createPresentSprite(); //プレゼント画像の作成
    void createPresentName(); //獲得プレゼント名の作成
    void changePresentData(); //プレゼント画像、名前の変更
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__LoginBounsPopupLayer__) */
