#ifndef __War__PresentMenuLayer__
#define __War__PresentMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "json11.hpp"
#include "cocos-ext.h"
#include "popupReceiveLayer.h"
#include "popupReceiveNoneLayer.h"
#include "PresentSprite.h"

//#define ORDER_RECEIVE_LAYER 20


USING_NS_CC;
USING_NS_CC_EXT;
using namespace network;
using namespace json11;

class PresentMenuLayer : public Layer, public ScrollViewDelegate
{
private:
    enum Tag{
        TAG_SCROLLVIEW=1,
        TAG_RECEIVE_ALL_BUTTON=3,
        TAG_RIGHT_ARROW=123,
        TAG_LEFT_ARROW,
        TAG_PAGE_NUM,
        TAG_PAGER_BACK,
    };
    
    enum Order{
        ORDER_SCROLLVIEW=10,
        ORDER_PRESENT_ALL=11,
        ORDER_PAGER_BACK=20,
    };
    
    //ページャ再生成 ページャとは・・・ < 1 2 3 4 > みたいなページを変えるアレ
    void recreatePager(int present_cont_all, int offset, bool has_page_next, bool has_page_prev) ;
    
    //ボタンが委譲する処理を返す
    function<void(Ref *sender, cocos2d::ui::Widget::TouchEventType type)> getFunctionByArrowTag(int tag) ;
    
    EventListenerTouchOneByOne *listener;
    HmsVector<PresentSprite *> presents; //プレゼント画像
    int _present_count_now_page; //プレゼントの数
    int _cell_size; //セルのサイズ
    
    int _offset ; //ページ表示の始まりの基準
    const int kListCellMaxCountInPage = 99; //リスト中に何個あるか
    
public:
    PresentMenuLayer() ;
    
    float ratio;
    bool touchButtonFlg;
    
    virtual bool init();
    CREATE_FUNC(PresentMenuLayer);
    
    Layer* createList(); //リストの作成
    void showSelectPage(); //データの数により表示ページの切り替え
    void createReceiveAllButton(); //まとめて受け取るボタンの作成
    
    void initEventListener(); //イベントリスナーの設定
    void createScrollView(); //スクロールビューの作成
    void initSprite(Json json); //スプライトの作成
    
    //プレゼントデータ通信処理
    void getPresentDataRequest();
    void presentDateCallback(HttpClient *sender, HttpResponse *response);

    //プレゼント受け取り更新処理
    void presentReceiveRequest(string tag);
    void presentReceiveCallback(HttpClient *sender, HttpResponse *response);
    
    //プレゼント一括受け取り更新処理
    void presentReceiveAllRequest();
    void presentReceiveAllCallback(HttpClient *sender, HttpResponse *response);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //スクロールの下端についたらtrueのフラグ
    bool _scroll_layer_lower_end_flg ;
    
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PresentMenuLayer__) */
