#include "QuestExplainSprite.h"

QuestExplainSprite::QuestExplainSprite()
:_quest_id()
,_quest_level()
,_quest_name()
,_quest_comment()
,_new_flg(false)
,_achieve_count(0)
{
}

QuestExplainSprite* QuestExplainSprite::create(json11::Json json){
    
    QuestExplainSprite *sprite = new QuestExplainSprite();
    
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool QuestExplainSprite::init(json11::Json json)
{
    if ( !Sprite::initWithFile("init/Home/Quest/story_title_l.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void QuestExplainSprite::onEnter()
{
    Sprite::onEnter();
    
    //キャラ画像の表示
    showCharaSprite();
    
    //New画像の表示
    showNewSprite();
    
    //New画像のアクション
    actionNewSprite();
    
    //クエスト名の表示
    showQuestName();
    
    //クエストコメントの表示
    showQuestComment();
    
    //達成画像の表示
    showAchieveStar();
}

void QuestExplainSprite::setupData(json11::Json json)
{
    //データの設定
    setQuestId(json["quest_id"].string_value());
    setQuestLevel(json["quest_level"].string_value());
    setQuestName(json["name"].string_value());
    setQuestNewFlg(json["new_flg"].bool_value());
    setQuestCommnet(json["comment"].string_value());
    setQuestAchieveCount(json["achieve_count"].int_value());
    
//    HMSLOG("Explain quest_id = %s, quest_level = %s, quest_name = %s, quest_new_flg = %d, quest_comment = %s", _quest_id.c_str(), _quest_level.c_str(), _quest_name.c_str(), _new_flg, _quest_comment.c_str());
}

void QuestExplainSprite::showQuestName()
{
    //クエスト名
    auto quest_name = HMSFontUtil::createWithTTFBySystemFontArgument(_quest_name.c_str(), FONT_NAME_SYSTEM, 44);
    quest_name->enableOutline(Color4B(0, 0, 0, 255), 3);
    quest_name->setAnchorPoint(Point(0, 0.5));
    quest_name->setPosition(Point(190, 150));
    this->addChild(quest_name);
}

void QuestExplainSprite::showCharaSprite()
{
    if (_quest_id != "") {
        string chara_path = "download/Quest/quest_stage/story_title_l_" + _quest_id + ".png";
        auto chara_sp = Sprite::create(chara_path);
        chara_sp->setAnchorPoint(Point(0, 0));
        chara_sp->setPosition(Point(22, 24));
        this->addChild(chara_sp, 2);
    }
}

void QuestExplainSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Quest/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(190, 196));
        this->addChild(new_sp, 10, EXPLAIN_NEW_TAG);
    }
}

void QuestExplainSprite::showQuestComment()
{
    //説明
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_quest_comment.c_str(), FONT_NAME_SYSTEM, 21);
    comment_label->setDimensions(416, 110);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setAnchorPoint(Point(0, 0));
    comment_label->setPosition(Point(190, 8));
    this->addChild(comment_label, 3);
}

void QuestExplainSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(EXPLAIN_NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}

void QuestExplainSprite::showAchieveStar()
{
    if (_achieve_count > 0 && _achieve_count <= 3)
    {
        //達成画像の表示
        auto achieve_sp = Sprite::create("init/Home/Quest/star" + toString<int>(_achieve_count) + ".png");
        achieve_sp->setAnchorPoint(Point(0.5, 0));
        achieve_sp->setPosition(Point(560, 120));
        this->addChild(achieve_sp, 10);
    }
}