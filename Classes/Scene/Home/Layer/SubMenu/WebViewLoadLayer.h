#ifndef __war__WebViewLoadLayer__
#define __war__WebViewLoadLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class WebViewLoadLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    Sprite* _bg; //背景画像
    Sprite* _background_black; //ロード時に背景を黒くするためのスプライト
    bool _first_load; //最初の読み込み時
    Sprite* _title_bg; //タイトル背景
    
    string _url; //表示URL
    string _page_title; //ページのタイトル
    
    /*
     onJSCallbackはWebViewからアプリを操作するための関数
     html側では 
     <a href="ccjs://[操作名]">適当な記述</a>
     または
     <a href="ccjs://[操作名]:[何らかの値]">適当な記述</a>
     のように記述します。
     
     アプリ側では、onJSCallbackが呼び出されるので
     onJSCallbackの第二引数のurlを解析して何らかの処理を実装します。
     
     ・実装例
     外部ブラウザの立上げ <a href="ccjs://openUrl:www.google.co.jp">外部ブラウザを立ち上げる</a>
     友達申請 <a href="ccjs://friendDemand:12345678">友達申請する</a>
     WebViewを閉じる    <a href="ccjs://close:">WebViewを閉じる</a>
     進む <a href="ccjs://goForward:">進む</a>
     戻る <a href="ccjs://goBack:">戻る</a>
     更新 <a href="ccjs://reload:">更新する</a>
     
     */
    void onJSCallback(experimental::ui::WebView *sender, const std::string &url);
    string eraseSchemePlace(string scheme, string url) ; //urlからscheme部分を取り除く
public:
    float ratio;
    
    static WebViewLoadLayer* create(string url, string page_title);
    virtual bool init(string url, string page_title);
//    CREATE_FUNC(WebViewLoadLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景画像の作成
    void createLoading(); //ローディングスプライトの表示
    void createTitle(); //タイトル画像の作成
    void createWebView(); //ウェブページの作成
    
    //ロードアニメ
    void showLoadingEffect(Node *parent_node, string filename,const Point& center_position, float scale) ;
    
    void popupAction(); //表示アクション
    void deleteAction(); //閉じるアクション
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //初期実行関数
    void onEnter();
};

#endif /* defined(__war__WebViewLoadLayer__) */