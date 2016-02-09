#ifndef __War__FriendSearchLayer__
#define __War__FriendSearchLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "extensions/cocos-ext.h"

#define FIRST_1_X_POS 100
#define BUTTON_X_POS 420

USING_NS_CC;
USING_NS_CC_EXT;
using namespace network;
using namespace json11;

class FriendSearchLayer : public Layer, public ui::EditBoxDelegate
{
private:
    string input_text; //入力文字列
    
public:
    float ratio;
    int data_count;
    
    virtual bool init();
    CREATE_FUNC(FriendSearchLayer);
    
    void createBg(); //文字入力は背景の作成
    void createEditBox(); //文字入力フォームの作成
    void createButton(); //検索ボタンの作成
    
    void onEnter(); //初期実行関数
    
    //通信処理
    void friendSearchRequest();
    void friendSearchCallback(HttpClient *sender, HttpResponse *response);
    
    /* EditBox処理 */
    virtual void editBoxEditingDidBegin(ui::EditBox *editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox *editBox);
    virtual void editBoxTextChanged(ui::EditBox *editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox *editBox);
};

#endif /* defined(__War__FriendSearchLayer__) */
