#include "QuestionLayer.h"

bool QuestionLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("QuestionLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void QuestionLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void QuestionLayer::setupImage(){
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("お問い合わせ", "SubMenu", this);
    
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("お問い合わせ", FONT_NAME, 40);
    label->setScale(ratio);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 1));
    this->addChild(label, 10);
    
    
    //お問い合わせ遷移ボタン
    auto questionbt = MenuItemImage::create("init/Home/mbar/back.png", "init/Home/mbar/back.png", CC_CALLBACK_1(QuestionLayer::questionPage, this));
    questionbt->setScale(ratio);
    questionbt->setTag(BACK_BUTTON_TAG);
    auto Menu2 = Menu::create(questionbt, nullptr);
    Menu2->setPosition(convertFromDisplaySize(Point(52, 754), 0.5, 1));
    this->addChild(Menu2, 2, BACK_BUTTON_TAG);
    
    //ブラウザ起動
    NativeBridge::openURL(QUESTION_B_URL);
}

void QuestionLayer::questionPage(Ref* sender){
    
}

void QuestionLayer::changePage(Ref* sender){
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("SubMenu");
    HMSLOG("touch back!");
}
