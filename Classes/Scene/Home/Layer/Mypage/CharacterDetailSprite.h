#ifndef __war__CharacterDetailSprite__
#define __war__CharacterDetailSprite__

#include "cocos2d.h"
#include "NormalScene.h"

#define CLOSE_BUTTUN_TAG 1
#define TAG_LOCK_MENU 2
#define TAG_LOCK_MENU_GRAY 3
USING_NS_CC;

/**
 *
 * このコードはCharacterDetailを流用するために
 * 変なことをしているので流用しないようにして下さい
 *
 **/
class CharacterDetailSprite : public Layer
{
private:
    EventListenerTouchOneByOne *listener ;
    //キャラクターリストID
    int _character_list_index;
    
    ////キャラクター一覧////
    map<int, map<string, string> > character_list;
    map<int, map<string, string> > _active_skill_list; //
    map<int, map<string, string> > _passive_skill_list; //
    
    Sprite *_base_node ;
    
    //chara_list_idからcharacter_listを割り出す
    int getCharalistIndexByCharaListId(int chara_list_id) ;
    
public:
    
    float ratio;
    
    static CharacterDetailSprite* create() ;
    bool init() ;
    
    void setupImage(int chara_list_id);
    
    bool getLockData(int chara_list_id);
    
    void setLockData(int chara_list_id);
    
    void removeLockData(int chara_list_id);
    
    //タップしたポイントのタグを調べて返す。
    int touchButtun(Point touchPoint);
    
    void showNew(Sprite* card);
    
    void showParam(Sprite* card, string param, string sortParam);
    
    void showLock(Sprite* card);
    
    void popupAction();
    
    void deleteAction(Ref* sender);
    
    //ロックデータのボタンの色と保存状態を切り替える
    void toggleLockData(Ref* sender) ;
};

#endif /* defined(__war__CharacterDetailSprite__) */
