#ifndef __War__BootyPopupSprite__
#define __War__BootyPopupSprite__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "HttpConnect.h"
#include "json11.hpp"

#define ITEM_1_POS_X 120
#define ITEM_1_POS_Y 295
#define ITEM_DISTANCE_X 260
#define ITEM_DISTANCE_Y 150

#define ITEM_BOX_WIDTH 510
#define ITEM_BOX_HEIGHT 350

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class BootyPopupSprite : public Sprite
{
private:
    
    enum BootyPopupSpriteTag{
        TAG_REWARD_ITEM 
    };
    ClippingNode *_clipper ;
    
    Json json;
	Json _reward_item_list; //報酬リスト
    int _all_item_count; //獲得報酬数

//    vector<Json > _item_list; //アイテムデータリスト
    HmsVector<Json > _item_list; //アイテムデータリスト
    HmsVector<Sprite *> _item_sprite; //アイテム画像
    
    //今表示している報酬のまとまりのindex ※報酬は4つずつ表示する
    int _now_container_index ;
    
    //まとまりの最後のindex
    int _max_container_index;
    
public:
    float ratio;
    
    BootyPopupSprite(); //コンストラクタ
    
    static BootyPopupSprite* create(const string filename, Json json);
    virtual bool init(const string filename, Json json);
    
    void loadData(Json json); //初期データの読み込み
    
    void createItemList(); //ランク、報酬合わせたアイテムリストの作成
    void createItemSprite(); //獲得アイテム画像の作成
    void createClippingNode(); //ClippingNodeの作成
    void createLabelByRewardType(string reward_type) ; //報酬取得タイプ理由の表示
    string getSpritePass(string item_type, string related_primary_id = 0); //アイテムの種類によりスプライトのパスを取得
    
    void showBootySprites(); //戦利品を表示する
    
    void showItemSprite(int container_index); //アイテム画像の表示
    void showItemName(int container_index); //獲得アイテムの名前の表示
    void showRewardDescription(int container_index); //報酬の詳細説明を表示

    bool hasNextContainer(); //次に表示する報酬があるか
    void slideNextContainer(); //次のまとまりに切り替える
    bool _now_swipe; 
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__BootyPopupSprite__) */
