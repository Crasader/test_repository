#include "BannerSprite.h"

BannerSprite::BannerSprite()
:_event_end(false)
,_server_diff_time(0)
,_end_time(0)
{
    
}

BannerSprite* BannerSprite::create(string image_path)
{
    BannerSprite *sprite = new BannerSprite();
    sprite->setCascadeOpacityEnabled(true) ;
    
    if (sprite && sprite->init(image_path))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BannerSprite::init(string image_path)
{
    
    if (!initSwitch(image_path)) {
        return false;
    }
    CCLOG("BannerSprite::init()");
    
    return true;
}

//初期化成功時にtrueを返す
bool BannerSprite::initSwitch(string image_path){
    
    _extension_type = (ExtensionType)getExtensionTypeByFilename(image_path) ;
    
    switch (_extension_type) {
        case PLIST:
        {
            SpriteFrame *sprite_frame = getSpriteFrame(image_path);
            if(Sprite::initWithSpriteFrame(sprite_frame)){
                return true ;
            }
        }
            break;
            
        case PNG:
        {
            if (Sprite::initWithFile(image_path)) {
                return true;
            }
        }
            break;
        case EXCEPTION:
        {
            //画像が見つからない場合の仮画像
            if ( Sprite::initWithFile("init/Home/Common/bt_close.png")) {
                return true;
            }
        }
            break;
        default:
            break;
    }
    
    return false ;
}

void BannerSprite::onEnter()
{
    Sprite::onEnter();
    
    if(_extension_type == ExtensionType::PLIST){
        //アニメーションを動かす
        this->runAction(_repeat_forever) ;
    }
}

SpriteFrame *BannerSprite::getSpriteFrame(string filename)
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
    
    //永続的に発生させる
    _repeat_forever = RepeatForever::create(Sequence::create(animate, DelayTime::create(1.6f), nullptr));
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    SpriteFrame* spriteFrame = animFrames.front() ;
    
    return spriteFrame ;
}

BannerSprite::ExtensionType BannerSprite::getExtensionTypeByFilename(string file_path)
{
    if(FileUtils::getInstance()->isFileExist(file_path) == false) {
        return ExtensionType::EXCEPTION ;
    }
    
    //file_pathに拡張子名が含まれているか確認しタイプを変えす
    if( file_path.find( "plist", 0 ) != string::npos ) {
        return ExtensionType::PLIST ;
    }else if( file_path.find( "png", 0 ) != string::npos ){
        return ExtensionType::PNG ;
    }else{
        return ExtensionType::EXCEPTION ;
    }
}

void BannerSprite::addDisplayEndDate(string end_date)
{
    if(end_date == "0000-00-00 00:00:00" || end_date == ""){
        return ;
    }
    
    //終了時間を取得
    struct tm tm;
    strptime(end_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    _end_time = mktime(&tm);
    
    _server_diff_time = SaveData::loadInt("server_diff_time");
    
    //時間表示の背景
    _end_date_back = Sprite::create("init/Home/Common/bk_time.png") ;
    _end_date_back->setVisible(false) ;
    _end_date_back->setAnchorPoint(Point(0, 1)) ;
    _end_date_back->setPosition(Point(0, this->getContentSize().height));
    _end_date_back->setCascadeOpacityEnabled(true) ;
    this->addChild(_end_date_back) ;
    
    //あと
    _prefix_time_text = HMSFontUtil::createWithTTFBySystemFontArgument("あと", FONT_NAME_SYSTEM, 24);
    _prefix_time_text->setHorizontalAlignment(TextHAlignment::RIGHT);
    _prefix_time_text->setAnchorPoint(Point(1, 0)) ;
    _prefix_time_text->setPosition(Point(_end_date_back->getContentSize().width/2-26 ,10));
    _end_date_back->addChild(_prefix_time_text) ;
    
    //時間で更新するラベル
    _end_date_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 35);
    _end_date_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _end_date_label->setAnchorPoint(Point(0.5, 0)) ;
    _end_date_label->setPosition(Point(_end_date_back->getContentSize().width/2, 10));
    _end_date_label->setTextColor(Color4B::RED) ;
    _end_date_back->addChild(_end_date_label) ;
    
    //"時間"や"分"を入れる
    _suffix_time_text = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 24);
    _suffix_time_text->setHorizontalAlignment(TextHAlignment::LEFT);
    _suffix_time_text->setAnchorPoint(Point(0, 0)) ;
    _suffix_time_text->setPosition(Point(_end_date_back->getContentSize().width/2+30 ,10));
    _suffix_time_text->setTextColor(Color4B::RED) ;
    _end_date_back->addChild(_suffix_time_text) ;
    
    this->schedule(schedule_selector(BannerSprite::updateTimeDisplay), 1.0f);
}

void BannerSprite::updateTimeDisplay(float dt){
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
    
    //端末時刻とサーバー時間の差を加える
    now_time = now_time - _server_diff_time;
    
    //終了までの残り時間
    time_t remain_time_second = _end_time - now_time;
    
//    HMSLOG("remain_time:%d, now_time:%d", remain_time_second, now_time) ;
    if (0 < remain_time_second)
    {
        _end_date_back->setVisible(true) ;
        int minute = (int)(remain_time_second / 60.0f); //分
        int hour = (int)(minute / 60.0f); //時間
        int day = (int)(hour / 24.0f); //日
        
        if(day >= 1){
            _end_date_label->setString(toString<int>(day)) ;
            _suffix_time_text->setString("日") ;
        } else if(hour >= 1){
            _end_date_label->setString(toString<int>(hour)) ;
            _suffix_time_text->setString("時間") ;
        }else{
            _end_date_label->setString(toString<int>(ceil(remain_time_second / 60.0f))) ;
            _suffix_time_text->setString("分") ;
        }
    } else{
        //イベント終了
        this->setEventEnd(true) ;
        
        _suffix_time_text->setString("") ;
        _end_date_label->setSystemFontSize(25) ;
        _end_date_label->setString("終了しました") ;
        _prefix_time_text->setString("") ;
        
        this->setColor(Color3B::GRAY) ;
        this->unschedule(schedule_selector(BannerSprite::updateTimeDisplay));
    }
}

void BannerSprite::echoLog(){
}