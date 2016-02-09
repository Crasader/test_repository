#include "BackgroundLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"

BackgroundLayer* BackgroundLayer::create(string background_path, int area_battle_flg)
{
    BackgroundLayer *node = new BackgroundLayer();
    if (node && node->init(background_path, area_battle_flg))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool BackgroundLayer::init(string background_path, int area_battle_flg)
{
    if ( !Layer::init() )
    {
        return false;
    }
    _background_path = background_path;
    _area_battle_flg = area_battle_flg;
    
    float ratio = getVisibleRatio();
    
    RenderTexture *all_back = RenderTexture::create(getContentSize().width,getContentSize().height);
    all_back->setAnchorPoint(Vec2(0.5f,0.5f));
    all_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    addChild(all_back,5000);
    all_back->begin();//書き込み開始
    
    //背景の表示
    Sprite *img = Sprite::create(_background_path);
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    //this->addChild(img,0);
    img->visit();
    
    //サイド背景の表示
    Sprite *img4_1 = Sprite::create("init/Home/Common/side.png");
    img4_1->setScale(ratio);
    img4_1->setAnchorPoint(Point(1,0));
    img4_1->setPosition(convertFromDisplaySize(Point(0, 0), 0.5, 0));
    img4_1->visit();
    
    Sprite *img4_2 = Sprite::create("init/Home/Common/side.png");
    img4_2->setScale(ratio);
    img4_2->setAnchorPoint(Point(0,0));
    img4_2->setPosition(convertFromDisplaySize(Point(640, 0), 0.5, 0));
    img4_2->visit();
    
    //上下背景
    Sprite *img3 = Sprite::create("init/Dungeon/design/pt.png");
    img3->setScale(ratio);
    img3->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    //this->addChild(img3,1);
    img3->visit();
    
    //エリアバトル中の表示
    if(_area_battle_flg==1) {
        //Label* label = HMSFontUtil::createWithTTFBySystemFontArgument("エリアバトル中", FONT_NAME_SYSTEM, 30);
        //label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        //this->addChild(label,2);
        Sprite *img = Sprite::create("init/Dungeon/design/during_areabattle.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        //this->addChild(img, 2);
        img->visit();
    }
    
    //HPゲージ背景の表示
    Sprite *img2_1 = Sprite::create("init/Dungeon/design/battle_base2_e.png");
    img2_1->setScale(ratio);
    img2_1->setPosition(convertFromDisplaySize(Point(320, 720), 0.5, 0.5));
    //this->addChild(img2_1,100);
    img2_1->visit();
    
    Sprite *img2_2 = Sprite::create("init/Dungeon/design/battle_base2_p.png");
    img2_2->setScale(ratio);
    img2_2->setPosition(convertFromDisplaySize(Point(320, 240), 0.5, 0.5));
    //this->addChild(img2_2,100);
    img2_2->visit();
    
    all_back->end();//書き込み終了
    
    return true;
}
