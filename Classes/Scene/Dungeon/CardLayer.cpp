/*charaをここにまとめて載せている*/

#include "CardLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "Chara.h"
#include "TurnManage.h"

bool CardLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    
    //batchNode = SpriteBatchNode::create("");
    
    
    
    
    
    return true;
}
void CardLayer::initialize(){
    
    
    
}

void CardLayer::whiteFilterCreate(vector<Point> field_place_vec){
    stopAllActions();
    if(white_filter!=nullptr) removeChild(white_filter);
    white_filter = nullptr;
    if(field_place_vec.empty()) return;
    
    white_filter = RenderTexture::create(getContentSize().width,getContentSize().height);
    white_filter->setAnchorPoint(Vec2(0.5f,0.5f));
    white_filter->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    addChild(white_filter,5000);
    white_filter->begin();//書き込み開始
    
    for(Point point : field_place_vec){
        //Sprite *img = Sprite::create("init/Dungeon/design/square_white.png");
        Sprite* img = Sprite::create();
        img->setTextureRect(Rect(0,0,100,100));//白い四角スプライト作成
        img->setAnchorPoint(Vec2(0.5f,0.5f));
        img->setScale(getVisibleRatio());
        img->setOpacity(70);
        img->setPosition(point);
        img->visit();
        //CCLOG("ポジション：%f：%f",point.x,point.y);
        
    }
    white_filter->end();//書き込み終了
    
    auto tinto = TintTo::create(0.6, 0,0,0);
    auto ease1 = EaseInOut::create(tinto, 2);
    auto spa1 = Spawn::create(tinto,ease1, NULL);
    
    auto tinto2 = TintTo::create(0.6, 255, 255, 255);
    auto ease2 = EaseInOut::create(tinto2, 2);
    auto spa2 = Spawn::create(tinto2,ease2, NULL);
    
    auto seq = Sequence::create(spa1, spa2, NULL);
    auto rep = RepeatForever::create(seq);
    
    white_filter->getSprite()->runAction(rep);
}

void CardLayer::whiteFilterCreatePlace(vector<pair<int,int>> field_place_vec){//場所選択用
    //DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    stopAllActions();
    if(white_filter!=nullptr) removeChild(white_filter);
    white_filter = nullptr;
    if(field_place_vec.empty()) return;
    
    white_filter = RenderTexture::create(getContentSize().width,getContentSize().height);
    white_filter->setAnchorPoint(Vec2(0.5f,0.5f));
    white_filter->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    addChild(white_filter,5000);
    white_filter->begin();//書き込み開始
    
    for(pair<int,int> place : field_place_vec){
        Sprite *img;
        if(place.first == TurnManage::getEnemyIndex()){//敵の場所の
            if(place.second == 1){
                img = Sprite::create("download/Dungeon/white_e_left.png");
                img->setPosition(convertFromDisplaySize(Point(195, 707), 0.5, 0.5));
            }else if(place.second == 0){
                img = Sprite::create("download/Dungeon/white_e_right.png");
                img->setPosition(convertFromDisplaySize(Point(447, 707), 0.5, 0.5));
            }else{
                img = Sprite::create("download/Dungeon/white.png");
                img->setPosition(convertFromDisplaySize(Point(571 - (place.second-2)*100, 576), 0.5, 0.5));
            }
        }else{//味方の場所の
            if(place.second == 1){
                img = Sprite::create("download/Dungeon/white_p_left.png");
                img->setPosition(convertFromDisplaySize(Point(195, 254), 0.5, 0.5));
            }else if(place.second == 0){
                img = Sprite::create("download/Dungeon/white_p_right.png");
                img->setPosition(convertFromDisplaySize(Point(447, 254), 0.5, 0.5));
            }else{
                img = Sprite::create("download/Dungeon/white.png");
                img->setPosition(convertFromDisplaySize(Point(71 + (place.second-2)*100, 392), 0.5, 0.5));
            }
        }
        
        //img->setTextureRect(Rect(0,0,100,100));//白い四角スプライト作成
        img->setAnchorPoint(Vec2(0.5f,0.5f));
        img->setScale(getVisibleRatio());
        img->setOpacity(70);
        img->visit();
        //CCLOG("ポジション：%f：%f",img->getPositionX(),img->getPositionY());
        
    }
    white_filter->end();//書き込み終了
    
    auto tinto = TintTo::create(0.6, 0,0,0);
    auto ease1 = EaseInOut::create(tinto, 2);
    auto spa1 = Spawn::create(tinto,ease1, NULL);
    
    auto tinto2 = TintTo::create(0.6, 255, 255, 255);
    auto ease2 = EaseInOut::create(tinto2, 2);
    auto spa2 = Spawn::create(tinto2,ease2, NULL);
    
    auto seq = Sequence::create(spa1, spa2, NULL);
    auto rep = RepeatForever::create(seq);
    
    white_filter->getSprite()->runAction(rep);
}