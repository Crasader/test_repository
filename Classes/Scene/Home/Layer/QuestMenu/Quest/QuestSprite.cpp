#include "QuestSprite.h"

QuestSprite::QuestSprite()
:_quest_id()
,_quest_level()
,_quest_name()
,_quest_comment()
,_new_flg(false)
,_clear_flg(false)
,_achieve_count(0)
,_quest_stage_data()
{
}

QuestSprite* QuestSprite::create(json11::Json json){
    
    QuestSprite *sprite = new QuestSprite();
    
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool QuestSprite::init(json11::Json json)
{
    if ( !Sprite::initWithFile("init/Home/Quest/story_title.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void QuestSprite::onEnter()
{
    Sprite::onEnter();
    
    //クエスト名の表示
    showQuestName();
    
    //クリア済み画像の表示
    showClearSprite();
    
    //New画像の表示
    showNewSprite();
    
    //Newアクション
    actionNewSprite();
    
    //キャラ画像の表示
    showCharaSprite();
    
    //達成画像の表示
    showAchieveStar();
}

void QuestSprite::setupData(json11::Json json)
{
    //データの設定
    setQuestId(json["quest_id"].string_value());
    setQuestLevel(json["quest_level"].string_value());
    setQuestName(json["name"].string_value());
    setQuestComment(json["comment"].string_value());
    setQuestNewFlg(json["new_flg"].bool_value());
    setQuestClearFlg(json["clear_flg"].bool_value());
    setQuestAchieveCount(json["achieve_count"].int_value());
    
    _quest_stage_data = json["quest_stage_data"].object_items();
    
//    HMSLOG("quest_id = %s, quest_level = %s, quest_name = %s, commente = %s, quest_new_flg = %d, quest_clear_flg = %d", _quest_id.c_str(), _quest_level.c_str(), _quest_name.c_str(), _quest_comment.c_str(), _new_flg, _clear_flg);
}

void QuestSprite::showQuestName()
{
    //クエスト名
    auto quest_name = HMSFontUtil::createWithTTFBySystemFontArgument(_quest_name.c_str(), FONT_NAME_SYSTEM, 44);
    quest_name->enableOutline(Color4B(0, 0, 0, 255), 3);
    quest_name->setAnchorPoint(Point(0, 0.5));
    quest_name->setPosition(Point(180, this->getContentSize().height / 2 - 6));
    this->addChild(quest_name);
}

void QuestSprite::showClearSprite()
{
    if (_clear_flg)
    {
        //クリア済みだった場合画像の表示
        auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
        clear_sp->setPosition(Point(242, 116));
        this->addChild(clear_sp, 10);
    }
}

void QuestSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Quest/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(174, 114));
        this->addChild(new_sp, 10, QUEST_NEW_TAG);
    }
}

void QuestSprite::showCharaSprite()
{
    if (_quest_id != "") {
        string chara_path = "download/Quest/quest/story_title_" + _quest_id + ".png";
        auto chara_sp = Sprite::create(chara_path);
        chara_sp->setAnchorPoint(Point(0, 0));
        chara_sp->setPosition(Point(26, 24));
        this->addChild(chara_sp, 2);
    }
}

void QuestSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(QUEST_NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}

void QuestSprite::showAchieveStar()
{
    if (_achieve_count > 0 && _achieve_count <= 3)
    {
        //達成画像の表示
        auto achieve_sp = Sprite::create("init/Home/Quest/star" + toString<int>(_achieve_count) + ".png");
        achieve_sp->setPosition(Point(520, this->getContentSize().height / 2));
        this->addChild(achieve_sp, 10);
    }
}

