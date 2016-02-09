#include "ui/CocosGUI.h"
#include "ContinueLayer.h"
#include "HmsGeometricFunction.h"
#include "HMSFontUtil.h"
#include "DungeonScene.h"
#include "HttpConnectHms.h"
#include "ContinueStoneShop.h"

bool ContinueLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    CCLOG("ContinueLayer::init()");
    ratio = getVisibleRatio();
    
    return true;
}

void ContinueLayer::showDetail(){
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    //イベントリスナーの作成
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    
    //下画面の表示
    bg_img = Sprite::create("download/Dungeon/bt_continue.png");
    bg_img->setScale(ratio);
    bg_img->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT *4/3), 0.5, 0.5));
    this->addChild(bg_img, 2);

    auto move = MoveTo::create(1.5f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT /2), 0.5, 0.5));
    //auto ease = EaseSineOut::create(move);
    auto ease = EaseBounceOut::create(move);
    bg_img->runAction(ease);
    
    //下画面のテキスト
    Label* footer_text1 = HMSFontUtil::createWithTTFBySystemFontArgument("コンティニュー？", FONT_NAME_SYSTEM, 43);
    footer_text1->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 65));
    bg_img->addChild(footer_text1, 2);
    
    //下画面のテキスト
    Label* footer_text2 = HMSFontUtil::createWithTTFBySystemFontArgument("宝玉　　　　　　　", FONT_NAME_SYSTEM, 25);
    footer_text2->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 140));
    footer_text2->setColor(Color3B::WHITE);
    bg_img->addChild(footer_text2, 2);
    
    string jewel_num = "　　"+to_string(dung->json_master["reborn_jewel"].int_value()) +"個　　　　　";
    Label* footer_text2_1 = HMSFontUtil::createWithTTFBySystemFontArgument(jewel_num, FONT_NAME_SYSTEM, 25);
    footer_text2_1->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 140));
    footer_text2_1->setColor(Color3B(0,255,255));
    bg_img->addChild(footer_text2_1, 2);
    
    Label* footer_text2_2 = HMSFontUtil::createWithTTFBySystemFontArgument("　　　　を使用して", FONT_NAME_SYSTEM, 25);
    footer_text2_2->setPosition(Point(bg_img->getContentSize().width / 2 , bg_img->getContentSize().height - 140));
    footer_text2_2->setColor(Color3B::WHITE);
    bg_img->addChild(footer_text2_2, 2);
    
    
    
    
    //下画面のテキスト
    Label* footer_text3 = HMSFontUtil::createWithTTFBySystemFontArgument("総大将のHPを全回復しますか？", FONT_NAME_SYSTEM, 25);
    footer_text3->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 172));
    bg_img->addChild(footer_text3, 2);
    
    //下画面のテキスト
    Label* footer_text4 = HMSFontUtil::createWithTTFBySystemFontArgument("所持している宝玉：　　　　", FONT_NAME_SYSTEM, 25);
    footer_text4->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 225));
    bg_img->addChild(footer_text4, 2);
    
    Label* footer_text4_1 = HMSFontUtil::createWithTTFBySystemFontArgument("　　　　　　　　"+SaveData::loadString("jewel")+"個", FONT_NAME_SYSTEM, 25);
    footer_text4_1->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 225));
    
    if(fromString<int>(SaveData::loadString("jewel"))==0){
        footer_text4_1->setColor(Color3B::RED);
    }else{
        footer_text4_1->setColor(Color3B::YELLOW);
    }
    bg_img->addChild(footer_text4_1, 2);
    
    /*Label* footer_text4_2 = HMSFontUtil::createWithTTFBySystemFontArgument("　　　　　　　　　　　　個", FONT_NAME_SYSTEM, 25);
    footer_text4_2->setPosition(Point(bg_img->getContentSize().width / 2, bg_img->getContentSize().height - 225));
    bg_img->addChild(footer_text4_2, 2);*/
    
    
    //『はい』ボタン
    Sprite* yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    Sprite* yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B::GRAY);
    
    yes_btn = MenuItemSprite::create(yes_off, yes_on, [this,dung](Ref* pSender){
        if(dung->getState() == "Waiting"){
            if(fromString<int>(SaveData::loadString("jewel"))>=dung->json_master["reborn_jewel"].int_value()){//復活可能な宝玉がある時
                
                yes_btn->setEnabled(false);
                
                
                string url = HOST;
                url += "battle/continue.php";
                string body = "";
                body += "user_master_id=" + SaveData::loadString("user_master_id");
                body += "&reborn_jewel=" + to_string(dung->json_master["reborn_jewel"].int_value());
                
                //送信に対してのコールバック
                HttpConnectHms::sendHttpRequest(url, [this,dung](HttpClient *client, HttpResponse *response){
                    if(response->isSucceed()){
                        string result = HttpConnectHms::getHttpResponse(this, client, response);
                        
                        if(result=="success"){//成功の場合
                            this->close();
                            dung->continueReborn();//復活
                            
                        }else{//失敗
                            yes_btn->setEnabled(true);//もう一度押せるように
                        }
                        
                        
                    }else{
                        CCLOG("continue送信失敗");
                        yes_btn->setEnabled(true);//もう一度押せるように
                    }
                }, body);
                
            }else{//復活可能な宝玉がない時
                
                auto layer = ContinueStoneShop::create(); //石(課金)購入画面
                dung->addChild(layer,1000);
                
                this->close();
            }
        }else{
            CCLOG("まだ終わってないから押せない");
        }
    });
    yes_btn->setPosition(Point((bg_img->getContentSize().width / 2) - (yes_btn->getContentSize().width / 2) - 20, (yes_btn->getContentSize().height / 2) + 120));
    
    
    //『いいえ』ボタン
    Sprite* no_off = Sprite::create("init/Home/Common/bt_no.png");
    Sprite* no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B::GRAY);
    no_btn = MenuItemSprite::create(no_off, no_on, [this,dung](Ref* pSender){
        this->close();
        dung->goResult();
    });
    no_btn->setPosition(Point((bg_img->getContentSize().width / 2) + (no_btn->getContentSize().width / 2) + 20, (no_btn->getContentSize().height / 2) + 120));
    
    
    Menu* pMenu = Menu::create(yes_btn, no_btn, NULL);
    pMenu->setPosition(Point(0, -100));
    bg_img->addChild(pMenu, 2);

}

void ContinueLayer::close()
{
    if(yes_btn != nullptr) yes_btn->setEnabled(false);//ボタン無効化
    if(no_btn != nullptr) no_btn->setEnabled(false);//ボタン無効化
    
    auto func = CallFunc::create([&](){
        static_cast<DungeonScene *>(getParent())->selectAbleEffect();
        //this->removeAllChildren();
        removeFromParent();
    });
    
    //スキル詳細のアニメーション
    auto seq = Sequence::create(MoveTo::create(0.1f, convertFromDisplaySize(Point(DISPLAY_WIDTH / 2 * -1, DISPLAY_HEIGHT /2 + 50), 0.5, 0.5)),
                                ScaleTo::create(0.1f, 0.0f * ratio),
                                FadeOut::create(0.1f),
                                nullptr
                                );

    
    bg_img->runAction(Spawn::create(seq,func, NULL));
}

