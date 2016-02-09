#include "TeamBattleEventRewardSprite.h"

TeamBattleEventRewardSprite::TeamBattleEventRewardSprite()
:_result_type()
{

}

TeamBattleEventRewardSprite* TeamBattleEventRewardSprite::create(json11::Json json)
{
    TeamBattleEventRewardSprite *sprite = new TeamBattleEventRewardSprite();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TeamBattleEventRewardSprite::init(json11::Json json)
{
    //データの設定
    setupData(json);
    
    string image_path ;
    if(_result_image_type == ResultImageType::WIN_PLACE) image_path = "init/Home/Event/TournamentEvent/bk_reward1.png" ;
    else if(_result_image_type == ResultImageType::LOSE_PLACE) image_path = "init/Home/Event/TournamentEvent/bk_reward2.png" ;
    
    if ( !Sprite::initWithFile(image_path) )
    {
        return false;
    }
    
    return true;
}

void TeamBattleEventRewardSprite::onEnter()
{
    Sprite::onEnter();
    
    //報酬画像の作成
    createRewardImage();
    
    //報酬一覧
    createRewardList();
}

void TeamBattleEventRewardSprite::setupData(json11::Json json)
{
    
    //データの設定
    setResultType(json["result_type"].string_value());
    
    if(_result_type == "win"){
        _result_image_type = ResultImageType::WIN_PLACE ;
    }else if(_result_type == "lose"){
        _result_image_type = ResultImageType::LOSE_PLACE ;
    }
    
    int count = json["count"].int_value();
    for(int i=0; i<count; i++){
        string str_index = toString<int>(i) ;
        _reward_data.push_back(json[str_index]) ;
    }
}


void TeamBattleEventRewardSprite::createRewardImage()
{
    string item_type = _reward_data[0]["item_type"].string_value() ;
    string id = _reward_data[0]["related_primary_id"].string_value() ;
    
    //報酬画像
    string filename = "";
    if(item_type=="chara") filename = "download/Chara/all400/" + id + ".png";
    else if(item_type=="gold") filename = "init/Present/item2_400.png";
    else if(item_type=="jewel") filename = "init/Present/item_400.png";
    
    Sprite *reward = Sprite::create(filename);
    
    if(_result_image_type == ResultImageType::WIN_PLACE){
        reward->setPosition(Point(117, 104));
        reward->setScale(0.38);
    } else if(_result_image_type == ResultImageType::LOSE_PLACE){
        reward->setPosition(Point(98, 70));
        reward->setScale(0.25);
    }
    this->addChild(reward, 1);
}

void TeamBattleEventRewardSprite::createRewardList()
{
    string reward_info = "" ;
    for(vector<json11::Json>::iterator iterator = _reward_data.begin(); iterator != _reward_data.end(); iterator++){
        if(iterator != _reward_data.begin())reward_info += "\n" ;
        
        json11::Json reward_data = *iterator;
        string item_type = reward_data["item_type"].string_value() ;
        string item_name = "" ;
        if(item_type == "jewel")item_name = "宝玉" ;
        else if(item_type == "gold")item_name = "資金" ;
        else if(item_type == "chara")item_name = reward_data["chara_name"].string_value() ;
        
        reward_info += item_name ;
        reward_info += "×" ;
        reward_info += reward_data["count"].string_value() ;
    }
    
    //行数に応じてフォントサイズを変える(基本サイズは40pxで入りきらない場合は行数で文字の入る部分の高さを割る)
    float font_size = 36;
    if(_result_image_type == ResultImageType::WIN_PLACE) {
        if(_reward_data.size() >= 4){
            font_size = 133 / _reward_data.size() ;
        }
    }else{
        if(_reward_data.size() >= 3){
            font_size = 76 / _reward_data.size() ;
        }
    }
    
    Label *reward_label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_info, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(reward_info, font_size, 330));
    reward_label->setAnchorPoint(Point(0, 0.5)) ;
    reward_label->setHorizontalAlignment(TextHAlignment::LEFT);
    if(_result_image_type == ResultImageType::WIN_PLACE) {
        reward_label->setPosition(Point(229, 92));
    }else{
        reward_label->setPosition(Point(229, 60));
    }
    this->addChild(reward_label) ;
}

