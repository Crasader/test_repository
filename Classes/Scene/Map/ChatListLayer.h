#ifndef _CHAT_LIST_LAYER_H_
#define _CHAT_LIST_LAYER_H_

#include "cocos2d.h"
#include "HmsScrollLayer.h"
#include "Utility.h"
#include "HmsList.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "UserDetailContainFriendIdSprite.h"

using namespace json11;

USING_NS_CC;
using namespace std;

//チームチャット
class ChatListLayer : public Layer, public ScrollViewDelegate
{
protected:
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    HmsList chat_list;
    
    Json _json_chat_data ;
    string _chat_type;
    float _scroll_height ;
    int _unread_count;
    
    HmsVector<ui::Scale9Sprite *> _frame_list;
    HmsVector<ui::Scale9Sprite *> _all_frame_list;
    HmsVector<string> _user_master_id_list; //_all_frame_listと同じ添字の対応関係がないとおかしくなります。
    HmsVector<int> _chat_id_list;
public:
    float getScrollHeight() ;
    
    static ChatListLayer* create(Json json_user_data_list);
    virtual bool init(Json json_user_data_list);
    Json _json_user_data_list ;
    
    void onEnter();
    void setup();
    
    enum Zorder{
       Z_COMMENT_BOX,
       Z_COMMENT_HEAD,
       Z_RED,
    };
    
    enum Tag{
        T_THUMB,
        T_RED,
    };
    
    //発言一つのセルを生成　
    ui::Scale9Sprite *createMessageCell(string comment, string post_user_type, float now_height, string image_path, string user_name, string insert_date, bool found_flg) ;
    
    //日付表示スプライトを生成
    Sprite *createChangeDate(string change_date);
    
    //表示範囲の計算
    Size calcDimensions(string comment_text, float font_size) ;
    
    //未読の表示確認
    void checkUnreadComment(float dt);

    //タップ関連
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロールビューの作成
    void createScrollView() ;
    int getUnreadCount();
};

#endif
