#include "ResultAchievementLayer.h"

#define CROWN_TAG 10

bool ResultAchievementLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ResultAchievementLayer::init()");
    
    ratio = getVisibleRatio();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
//    listener->setSwallowTouches(true); //←つなぎ込みの為に仮に外しました。必要におうじて修正して下さい。
    listener->onTouchBegan = CC_CALLBACK_2(ResultAchievementLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ResultAchievementLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ResultAchievementLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ResultAchievementLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void ResultAchievementLayer::setup(HmsVector<string> achivement_txt_list,
                                   HmsVector<int> old_achievement_flg_list,
                                   HmsVector<int> new_achievement_flg_list)
{
    _achivement_txt_list = achivement_txt_list;
    _old_achievement_flg_list = old_achievement_flg_list;
    _new_achievement_flg_list = new_achievement_flg_list;
    
    createSprites();
}


void ResultAchievementLayer::createSprites()
{
    //背景
    Sprite *sprite = Sprite::create("init/Dungeon/result/popup_booty3.png");
    sprite->setScale(ratio);
    sprite->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(sprite);
    
    //達成条件と王冠マーク
    for(int i=0; i<3; i++) {
        int y = 475;
        if(i==0) y = 525;
        if(i==2) y = 425;
        
        string filename = "init/Dungeon/result/star_gray.png";
        if(_old_achievement_flg_list[i]==1) filename = "init/Dungeon/result/star.png";
        Sprite *sprite = Sprite::create(filename.c_str());
        sprite->setScale(ratio);
        sprite->setPosition(convertFromDisplaySize(Point(80, y), 0.5, 0.5));
        this->addChild(sprite, 0, CROWN_TAG+i);
    
        Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(_achivement_txt_list[i], FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_achivement_txt_list[i], 24, 504));
        label->setPosition(convertFromDisplaySize(Point(104, y), 0.5, 0.5));
        label->setScale(ratio);
        label->setAnchorPoint(Point(0, 0.5));
        this->addChild(label);
    }
    
//    this->setPositionX(-640*ratio);
    
    animation();
}

void ResultAchievementLayer::animation()
{
//    //自身のスライドイン
//    DelayTime *delay = DelayTime::create(0.2f);
//    MoveTo *move = MoveTo::create(0.3f, Point(0, 0));
//    this->runAction(Sequence::create(delay, move, NULL));
    
    //星マークの移動
    for(int i=0; i<3; i++) {
        if(_old_achievement_flg_list[i]==0 && _new_achievement_flg_list[i]==1) {
            CCLOG("star%d", i);
            
            int y = 475;
            if(i==0) y = 525;
            if(i==2) y = 425;

            Point target_point = convertFromDisplaySize(Point(80, y), 0.5, 0.5);
            Point start_point = convertFromDisplaySize(Point(320, 480), 0.5, 0.5);
            
            string filename = "init/Dungeon/result/star.png";
            Sprite *sprite = Sprite::create(filename.c_str());
            sprite->setScale(ratio*3.0f);
            sprite->setPosition(start_point);
            this->addChild(sprite, 1);
            
            DelayTime *delay = DelayTime::create(0.55f + 0.1f*i);
            MoveTo *move = MoveTo::create(0.3f, target_point);
            ScaleTo *scale = ScaleTo::create(0.3f, ratio);
            RotateTo *rotate = RotateTo::create(0.3f, 360*5);
            Action *action = Spawn::create(move, scale, rotate, NULL);
            CallFunc *expire1 = CallFunc::create([this, target_point, i, sprite]()
                                                 {
                                                     //SE
                                                     CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/lvup.mp3",false);
                                                     
                                                     //パーティクル
                                                     ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                                     particle->setAutoRemoveOnFinish(true);
                                                     particle->setPosition(target_point);
                                                     particle->setScale(ratio);
                                                     this->addChild(particle, 1);

                                                     //古い方の画像を消す
                                                     this->removeChildByTag(CROWN_TAG+i);
                                                     
                                                     //移動後のアニメーション
                                                     DelayTime *delay = DelayTime::create(1.0f);
                                                     int count = 1000;
                                                     JumpTo *jump = JumpTo::create(count, sprite->getPosition(), 10*getVisibleRatio(), count);
                                                     sprite->runAction(Sequence::create(delay, jump, NULL));
                                                 });//ラムダ式
            sprite->runAction(Sequence::create(delay, action, expire1, NULL));
        }
    }
}

void ResultAchievementLayer::onEnter()
{
    Layer::onEnter();
}

bool ResultAchievementLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ResultAchievementLayer!");
    //タップされたら削除
    return true;
}
void ResultAchievementLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ResultAchievementLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ResultAchievementLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
