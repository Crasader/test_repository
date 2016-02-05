#ifndef __war__HmsScrollLayer__
#define __war__HmsScrollLayer__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "NormalScene.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class HmsScrollLayer : public cocos2d::Layer , public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    int _cell_size; //セルの大きさ
    HmsVector<Sprite *> _item_list; //表示する画像のリスト
    function<void(int tag)> _func; //セル選択時に実行される関数
    int _top_distance; //タイトルバーとの間隔距離
    
public:
    HmsScrollLayer();
    ~HmsScrollLayer();
    
    static HmsScrollLayer* create(int cell_size, HmsVector<Sprite *> &item_list, function<void(int tag)> func, int top_distance = 0);
    virtual bool init(int cell_size, HmsVector<Sprite *> &item_list, function<void(int tag)> func, int top_distance);
//    CREATE_FUNC(HmsScrollLayer);
    
    float ratio;
    bool _btn_touch_flg; //ボタンタッチフラグ
    void createEventListener(); //イベントリスナーの作成
    
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    void disableSpriteTouch(int tag); //タグの画像をタッチさせなくする
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__HmsScrollLayer__) */
