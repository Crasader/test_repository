#ifndef __War__JewelSprite__
#define __War__JewelSprite__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "StoneShopMenuLayer.h"
#include "ShopMenuLayer.h"

USING_NS_CC;
using namespace std;

class JewelSprite : public Sprite
{
private:
    enum Order{
        ORDER_RECEIVE_LAYER=20,
    };
    
    Sprite* _jewel_sp; //宝玉画像
    string _name; //商品名
    string _price; //価格
    string _id; //商品ID
    
    string switchImageById(string product_id) ;
    
public:
    float ratio;
    static JewelSprite* create(string name, string price, string id);
    virtual bool init(string name, string price, string id);
    
    void createJewelImg(); //宝玉画像の作成
    void showJewelNum(); //宝玉の数の表示
    void showJewelPrice(); //宝玉の価格の表示
    void createButton(); //購入ボタンの作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__JewelSprite__) */
