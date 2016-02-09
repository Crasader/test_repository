#ifndef __war__TournamentEventBracketLayer__
#define __war__TournamentEventBracketLayer__

#include "cocos2d.h"
#include "TournamentEventBracketSprite.h"
#include "NormalScene.h"
#include "ui/CocosGUI.h"


USING_NS_CC;

class TournamentEventBracketLayer : public Layer
{
private:
    EventListenerTouchAllAtOnce *listener;
    
    HmsVector<Sprite *> _tournament_list; //トーナメント画像リスト
    json11::Json _bracket_data; //対戦表データ
    TournamentEventBracketSprite* _tournamentEventBracketSprite  ; //対戦表のスプライト
    float _touch_begin_bracket_scale ;
    
    bool isNowBattle();
public:
    TournamentEventBracketLayer() ;
    
    //タグ
    enum Tag{
        T_ARROW_LEFT,
        T_ARROW_BELOW,
        T_ARROW_UPPER_RIGHT,
        T_ARROW_LOWER_RIGHT,
        T_ARROW_UP,
        T_POPUP_LAYER = 100,
    };
    
    //ZOrder
    enum ZOrder{
        Z_BACK,
        Z_BRACKET,
        Z_RIBBON,
        Z_ARROW,
        Z_POPUP_LAYER = 100,
    };
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TournamentEventBracketLayer);
    
    void initArrowButtons() ;
    ui::Button *createButtonByTag(int tag) ;
    Action *createActionByTag(int tag) ;
    Point getPointByArrowTag(int tag) ;
    float getRotateByArrowTag(int tag) ;
    
    void updateArrowButtons() ;
    void updateArrowButtonByTag(int tag) ;
    void initEventListener(); //イベントリスナーの設定
    void createBracketList(); //トーナメント画像のリスト生成
    
    //通信処理
    void getTournamentEventInfoRequest();
    void tournamentEventInfoCallback(HttpClient *sender, HttpResponse *response);
    
    //ページを戻る処理
    void returnPage(string page_name, string popup_message) ;
    
    //タップ可能にする
    bool _tap_enable ;
    bool _flg_pinch_in_out_mode; //拡大縮小モード
    
    //タッチイベント処理
    void onTouchBegan(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event);
    void onTouchMoved(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event);
    void onTouchEnded(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event);
    void onTouchCancelled(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event);
    
    //ボタンタップ処理
    void touchEvent(Ref *sender, cocos2d::ui::Widget::TouchEventType type) ;
    TournamentEventBracketSprite::SwipeDirection _long_tap_swipe_direction ; //ロングタップ時のスワイプ方向
    void updateLongTapDetect(float dt); //ロングタップ判定
    void updateLongTap(float dt); //ロングタップ時処理
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventBracketLayer__) */
