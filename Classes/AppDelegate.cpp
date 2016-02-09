#include "AppDelegate.h"
#include "TitleScene.h"
#include "NormalScene.h"
#include "DungeonScene.h"
#include "ReplayScene.h"
#include "Cocos2dxFox.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    
#if(RAKUTEN_FLG)
    //FOX 楽天の場合アクセス解析無し
#elif (IF_ANDROID)
    FoxPlugin::sendConversion("default");
    
    //FOX アクセス解析
    FoxPlugin::sendStartSession();
#endif
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }
    

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = TitleScene::createScene();
    //auto scene = DungeonScene::createScene();
    //auto scene = NormalScene::createScene("Quest");
    //auto scene = ReplayScene::createScene();
    
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    SaveData::saveBool("background_flg", true);
    
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    // BGM・SE一時停止
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    // BGM・SE再開
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
    
#if(RAKUTEN_FLG)
    //FOX 楽天の場合アクセス解析無し
#elif (IF_ANDROID)
    //FOX アクセス解析
    FoxPlugin::sendStartSession();
#endif
    
}
