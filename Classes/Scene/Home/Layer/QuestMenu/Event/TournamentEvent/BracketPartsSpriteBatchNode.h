#ifndef __war__BracketPartsSpriteBatchNode__
#define __war__BracketPartsSpriteBatchNode__

#include "cocos2d.h"

USING_NS_CC;

class BracketPartsSpriteBatchNode : public SpriteBatchNode
{
private:
    //定数 画像の形状 0:空白 1:│ 2:─ 3:┴ 4:┌ 5:┐ 6:┘ 7:└
    static const std::vector<std::string> PARTS_PATH_NORMAL; //対戦表の画像の部品のパス
    static const std::vector<std::string> PARTS_PATH_RED; //対戦表の画像の部品のパス(マスクに使用)
    static const int PARTS_ROTATION[]; //対戦表画像の回転角度
    static const bool PARTS_FLIP_X_FLG[];//対戦表画像のx軸方向の逆転
    
    Sprite *_red_sprite;
public:
    BracketPartsSpriteBatchNode();
    
    static BracketPartsSpriteBatchNode* create(int parts_type, bool is_mask);
    virtual bool init(int parts_type, bool is_mask);
    
    void onEnter(); //初期実行関数
    
};


#endif /* defined(__war__BracketPartsSpriteBatchNode__) */
