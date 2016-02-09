
#ifndef __war__FieldLayer__
#define __war__FieldLayer__

#include "cocos2d.h"
#include "Chara.h"

USING_NS_CC;

class FieldLayer : public Layer
{
public:
    float ratio;
    
    vector<Point> field_position_list[2];//自分と敵のフィールドのカード置く位置のリスト
    vector<Point> hand_position_list[2];//自分と敵の手札のカード置く位置のリスト
    Point friend_position_list[2];//自分と敵のフレンドカードの位置のリスト
    Point cardSize;
    Point castle_point[2];
    Point deck_point[2];
    
    Sprite *cardSelectImage;//カードがそこに置ける時に表示する画像
   
public:
    virtual bool init();
    CREATE_FUNC(FieldLayer);
    
    bool placeAbleField(int field_index, int index);
    bool equalPositionType(Chara *tapped_chara,int field_index,int player_index);
    int getPositionIndex(Point tap_position, string field_type, int index);
    void fieldStateEffect();
    Chara* getCharaFieldIndex(int field_index, int index);
    Point getCharaPosition(Chara* chara);
    bool fieldBlankCheck(int field_index ,int index);
};



#endif /* defined(__war__FieldLayer__) */
