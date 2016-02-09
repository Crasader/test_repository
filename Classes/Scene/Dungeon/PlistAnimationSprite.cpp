#include "PlistAnimationSprite.h"
#include "cocos2d.h"


Sprite* PlistAnimationSprite::setData(Layer* layer, string filename,int loop_count, float frame_rate ,int z_order){
//plistアニメーションの設定を行ったSpriteを返すクラス
    
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename + ".plist", filename + ".png");
    
    auto anim = Animation::create();
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    char str[100] = {0};
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        
        anim->addSpriteFrame(frame);
    }
    anim->setDelayPerUnit(frame_rate);
    anim->setRestoreOriginalFrame(true);
    
    auto sprite = Sprite::createWithSpriteFrameName("01.png");//最初の画像
    layer->addChild(sprite, z_order);
    
    auto action = Animate::create(anim);
    
    
    if(loop_count == 0){//0の場合は無限ループ
        auto repeat = RepeatForever::create(action);
        sprite->runAction(repeat);
    }else{//1以上の場合は回数分再生
        auto repeat = Repeat::create(action,loop_count);
        sprite->runAction(Sequence::create(repeat, RemoveSelf::create(), NULL));
    }
    
    
    return sprite;
}






