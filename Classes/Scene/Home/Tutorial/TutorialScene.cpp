#include "TutorialScene.h"

Scene* TutorialScene::createScene()
{
    auto scene = TutorialScene::create();
    
    Layer* layer;
    
    if (SaveData::loadInt("tutorialStatus") != 0)
    {
        //チュートリアル10連ガチャの表示
        layer = BattleTutorialAfterLayer::create();
    }
    else
    {
        //最初に表示するページ
        layer = UserNameInsertLayer::create();
    }
    
    scene->addChild(layer, ZORDER_MAIN_LAYER, MAIN_LAYER_TAG);
    
    return scene;
}

bool TutorialScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    HMSLOG("TutorialScene::init()");
    
    ratio = getVisibleRatio();
    
    //背景画像の作成
    createBg();
    
    //両サイドの余白に画像を配置する
    cutLeftAndRightVisibleSpace() ;
    
    //瞬き用スケジュールの設定
    this->schedule(schedule_selector(TutorialScene::blinkTexture), 3.0f);

    //BGM再生
    this->runAction(Sequence::create(DelayTime::create(1.6f),
                    PlayBGM::create("download/BGM/05_deck_create.mp3"),
                    FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),
                    CallFunc::create([](){HMSLOG("volume:%f", SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                    );

    return true;
}

void TutorialScene::createBg()
{
    //背景画像の作成
    auto bg = Sprite::create("init/Tutorial/back2.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(bg, 1);
}

void TutorialScene::changePage(Layer* next_layer)
{
    //MAIN_LAYER_TAGのレイヤーが2つ以上だったらchangePageを終了する
    if(countChildByTag(MAIN_LAYER_TAG)>=2){
        return ;
    }
    
    auto main_layer = (Layer *)this->getChildByTag(MAIN_LAYER_TAG);
    
    if (next_layer)
    {
        //次のレイヤーを透明でセット
        next_layer->setOpacity(0);
        this->addChild(next_layer, ZORDER_MAIN_LAYER, MAIN_LAYER_TAG);
        
        //子ノードにも適用させる処理
        _setCascadeOpacityEnabled(main_layer, true);
        _setCascadeOpacityEnabled(next_layer, true);
        
        //ポップアップがある場合閉じる処理
        closePopupLayer(main_layer);
        
        auto fadeout = FadeOut::create(0.6f);
        auto removeFunc = CallFunc::create([main_layer](){
            main_layer->removeFromParent();
        });
        auto fadeInFunc = CallFunc::create([next_layer](){
            auto fadeIn = FadeIn::create(0.6f);
            next_layer->runAction(fadeIn);
        });

        main_layer->runAction(Sequence::create(fadeout, removeFunc, fadeInFunc, NULL));
    }
}

int TutorialScene::countChildByTag(int tag){
    int count = 0 ;
    for (int i=0; i<getChildrenCount(); i++) {
        Node *node = getChildren().at(i) ;
        if(node->getTag() == tag){
            count++ ;
        }
        
    }
    return count ;
}

void TutorialScene::changePageGachaAnimation(Layer* next_layer)
{
    auto main_layer = (Layer *)this->getChildByTag(MAIN_LAYER_TAG);
    
    if (next_layer)
    {
        //次のレイヤーを透明でセット
        next_layer->setOpacity(0);
        this->addChild(next_layer, ZORDER_MAIN_LAYER, MAIN_LAYER_TAG);
        
        //子ノードにも適用させる処理
        _setCascadeOpacityEnabled(main_layer, true);
        _setCascadeOpacityEnabled(next_layer, true);
        
        //ポップアップがある場合閉じる処理
        closePopupLayer(main_layer);
        
        auto fadeout = FadeOut::create(0.2f);
        auto removeFunc = CallFunc::create([main_layer](){
            main_layer->removeFromParent();
        });
        auto fadeInFunc = CallFunc::create([next_layer](){
            auto fadeIn = FadeIn::create(0.2f);
            next_layer->runAction(fadeIn);
        });
        
        main_layer->runAction(Sequence::create(fadeout, removeFunc, fadeInFunc, NULL));
    }
}

void TutorialScene::closePopupLayer(Layer* main_layer)
{
    //ポップアップが存在する場合閉じる処理
    auto popup_layer = main_layer->getChildByTag(ALERT_LAYER_TAG);
    
    if (popup_layer)
    {
        _setCascadeOpacityEnabled(popup_layer, true);
        auto scale = ScaleTo::create(0.1f, 0);
        
        auto func = CallFunc::create([popup_layer](){
            popup_layer->removeFromParent();
        });
        
        popup_layer->runAction(Sequence::create(scale, func, NULL));
    }
}

void TutorialScene::blinkTexture(float dt)
{
    auto message_bg = (Sprite *)this->getChildByTag(MAIN_LAYER_TAG)->getChildByTag(MESSAGE_SPRITE_TAG);
    
    if (message_bg)
    {
        //メッセージ（瞬き用の画像）があった場合
        closeEyeTexure();
    }
    
}

void TutorialScene::openEyeTexture(float dt)
{
    //目が開く画像の処理
    auto message_bg = (Sprite *)this->getChildByTag(MAIN_LAYER_TAG)->getChildByTag(MESSAGE_SPRITE_TAG);
    
    if (message_bg)
    {
        message_bg->setTexture("init/Tutorial/navi1.png");
    }
}

void TutorialScene::closeEyeTexure()
{
    //目が閉じる画像の処理
    auto message_bg = (Sprite *)this->getChildByTag(MAIN_LAYER_TAG)->getChildByTag(MESSAGE_SPRITE_TAG);
    
    if (message_bg)
    {
        message_bg->setTexture("init/Tutorial/navi2.png");
    }
    
    this->scheduleOnce(schedule_selector(TutorialScene::openEyeTexture), 0.2f);
}

void TutorialScene::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void TutorialScene::cutLeftAndRightVisibleSpace()
{
    int space_width = getVisibleSpaceWidth();
    
    //左右の余白がある場合、余白部分を黒くする
    if(space_width > 0){
        //左の柄
        Sprite *left_sprite = Sprite::create("init/Home/Common/side.png");
        left_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        left_sprite->setAnchorPoint(Point(1,0));
        left_sprite->setScale(ratio);
        left_sprite->setPosition(convertFromDisplaySize(Point(0, 0), 0.5, 0));
        this->addChild(left_sprite);
        
        //右の柄
        Sprite *right_sprite = Sprite::create("init/Home/Common/side.png");
        right_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        right_sprite->setAnchorPoint(Point(0,0));
        right_sprite->setScale(ratio);
        right_sprite->setPosition(convertFromDisplaySize(Point(640, 0), 0.5, 0));
        this->addChild(right_sprite);
        
    }
}