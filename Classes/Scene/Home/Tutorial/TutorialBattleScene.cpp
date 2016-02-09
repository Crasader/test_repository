#include "TutorialBattleScene.h"

Scene* TutorialBattleScene::createScene()
{
    auto scene = Scene::create();
    auto layer = TutorialBattleScene::create();
    scene->addChild(layer);
    return scene;
}

bool TutorialBattleScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TutorialBattleScene::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TutorialBattleScene::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TutorialBattleScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TutorialBattleScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TutorialBattleScene::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TutorialBattleScene::createBg()
{
    //背景画像の作成
    auto bg = Sprite::create("download/Dungeon/battle_bk.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(bg, 1);
    
    auto bg_base = Sprite::create("init/Dungeon/design/battle_base.png");
    bg_base->setScale(ratio);
    bg_base->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(bg_base, 2);
}

void TutorialBattleScene::showMessage()
{
    //ページ内容の表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("バトルチュートリアル", FONT_NAME_SYSTEM, 30);
    label->setScale(ratio);
    label->setPosition(convertFromDisplaySize(Point(320, 600), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    addChild(label, 3);
}

void TutorialBattleScene::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの作成
    createEventLitener();
    
    //背景画像の作成
    createBg();
    
    //ページ内容の表示
    showMessage();
}

bool TutorialBattleScene::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("TutorialBattleScene! touchBegan");
    return true;
}

void TutorialBattleScene::onTouchMoved(Touch *touch, Event *event)
{
}

void TutorialBattleScene::onTouchEnded(Touch *touch, Event *event)
{
    //チュートリアルステータスの変更
    SaveData::saveInt("tutorialStatus", 2);
    
    //シーンの切り替え
    auto scene = TutorialScene::createScene();
    auto transition = TransitionZoomFlipX::create(1.0f, scene);
    Director::getInstance()->replaceScene(transition);
}