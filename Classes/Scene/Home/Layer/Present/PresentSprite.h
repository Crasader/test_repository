#ifndef __War__PresentSprite__
#define __War__PresentSprite__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"

USING_NS_CC;

class PresentSprite : public Sprite
{
private:
    CC_SYNTHESIZE(int, _primaryId, PrimaryId); //プレゼントプライマリーID
    CC_SYNTHESIZE(string, _message, Message); //プレゼントの詳細
    CC_SYNTHESIZE(string, _item_type, ItemType); //アイテムの種類
    CC_SYNTHESIZE(string, _related_primary_id, RelatedPrimaryId); //アイテムのプライマリーID
    CC_SYNTHESIZE(int, _present_count, PresentCount); //プレゼント数
    CC_SYNTHESIZE(string, _chara_name, CharaName); //プレゼントタイプがcharaだった場合のキャラ名
    CC_SYNTHESIZE(string, _insert_date, InsertDate); //プレゼント取得日時
    
    Sprite* present_sp; //プレゼント画像
    Label* present_name_label; //プレゼントの名前
    void changeData(); //アイテムタイプにより表示画像を変える
    
public:
    PresentSprite(); //コンストラクタ
    
    float ratio;
    void createPresentSprite(); //プレゼント画像の作成
    void showPresentSprite(); //プレゼント画像の表示
    void createPresentName(); //プレゼント名の作成
    void showPresentName(); //プレゼント名の表示
    
    void showData(); //プレゼント情報を表示
    
    void setupData(json11::Json json); //データの設定
    void parseInsertDate(); //insert_dateの文字列分解
    
    void onEnter(); //初期実行関数
    
    static PresentSprite* create();
    virtual bool init();
};

#endif /* defined(__War__PresentSprite__) */
