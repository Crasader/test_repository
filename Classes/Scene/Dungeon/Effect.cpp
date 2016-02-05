#include "Effect.h"
#include "HmsGeometricFunction.h"

Scene* Effect::createScene()
{
    auto scene = Scene::create();
    auto layer = Effect::create();
    scene->addChild(layer);
    return scene;
}

bool Effect::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("Effect::init()");
    
    return true;
}

void Effect::setData(Layer* layer, string filename, Vec2 position, float scale)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename);
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(scale);
    sprite->setPosition(position);
    layer->addChild(sprite, 10000);
    
    auto func = CallFunc::create([sprite]()
    {
        sprite->removeFromParentAndCleanup(true);
    });
    
    sprite->runAction(Sequence::create(animate, func, nullptr));
}

void Effect::setData(Layer *obj, string filename, int count, Vec2 position, float ratio_, int z){
    setData(obj, filename, count, position, ratio_, z, 1) ;
}

void Effect::setData(Layer *obj, string filename, int count, Vec2 position, float ratio_, int z, int roop_count)
{
    SpriteFrameCache *cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename + ".plist", filename + ".png");
    
    Sprite *_sprite1 = Sprite::createWithSpriteFrameName("01.png");
    
    SpriteBatchNode *spritebatch = SpriteBatchNode::create(filename + ".png");
    spritebatch->addChild(_sprite1);
    spritebatch->setPosition(position);
    spritebatch->setScale(ratio_*getVisibleRatio());
    obj->addChild(spritebatch, z);
    
    Vector<SpriteFrame*> animFrames(48);
    
    char str[100] = {0};
    for(int i=1; i<=roop_count; i++){
        for(int j=1; j<=count; j++)
        {
            sprintf(str, "%02d.png", j);
            //HMSLOG("str=%s", str);
            SpriteFrame *frame = cache->getSpriteFrameByName( str );
            if (frame) {
                animFrames.pushBack(frame);
            }
            //animFrames.pushBack(frame);
        }
    }
    
    Animation *animation = Animation::createWithSpriteFrames(animFrames, 0.05f);
    //_sprite1->runAction(Animate::create(animation));
    Animate *animate = Animate::create(animation);
    //CallFunc *expire1 = CallFunc::create(_sprite1, callfunc_selector(Sprite::removeFromParent));
    CallFunc *expire1 = CallFunc::create([_sprite1]()
                                         {
                                             _sprite1->removeFromParentAndCleanup(true);
                                         });
    CallFunc *expire2 = CallFunc::create(this, callfunc_selector(Effect::removeFromParent));
//    CallFunc *expire2 = CallFunc::create(CC_CALLBACK_0(Effect::removeFromParent,this));
    ActionInterval *seq = Sequence::create(animate, expire1, expire2, nullptr);
    _sprite1->runAction(seq);
}