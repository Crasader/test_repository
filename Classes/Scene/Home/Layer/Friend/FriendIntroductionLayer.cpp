#include "FriendIntroductionLayer.h"

bool FriendIntroductionLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("FriendIntroductionLayer::init()");
    
    return true;
}

void FriendIntroductionLayer::setupData()
{
    //ブラウザ起動
    NativeBridge::openURL("http://www.yahoo.co.jp/");
    
//    auto webView = cocos2d::experimental::ui::WebView::create();
//    webView->setAnchorPoint(Point(0.5, 0.5));
//    webView->setContentSize(Size(320, 320));
//    webView->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 1));
//    webView->loadURL("http://www.yahoo.co.jp/");
//    this->addChild(webView, 1);

}
