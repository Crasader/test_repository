#include "TournamentEventRewardSprite.h"

TournamentEventRewardSprite::TournamentEventRewardSprite()
:_ranking()
{

}

TournamentEventRewardSprite* TournamentEventRewardSprite::create(json11::Json json)
{
    TournamentEventRewardSprite *sprite = new TournamentEventRewardSprite();
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventRewardSprite::init(json11::Json json)
{
    //データの設定
    setupData(json);
    
    string image_path ;
    if(_ranking_image_type == RankingImageType::FIRST_PLACE) image_path = "init/Home/Event/TournamentEvent/bk_reward1.png" ;
    else if(_ranking_image_type == RankingImageType::SECOND_PLACE) image_path = "init/Home/Event/TournamentEvent/bk_reward2.png" ;
    else if(_ranking_image_type == RankingImageType::UNDER_THIRD_PLACE) image_path = "init/Home/Event/TournamentEvent/bk_reward3.png" ;
        
    if ( !Sprite::initWithFile(image_path) )
    {
        return false;
    }
    
    return true;
}

void TournamentEventRewardSprite::onEnter()
{
    Sprite::onEnter();
    
    //報酬画像の作成
    createRewardImage();
    
    //報酬を獲得できる順位の表示
    createPlaceLabel();
    
    //報酬一覧
    createRewardList();
}

void TournamentEventRewardSprite::setupData(json11::Json json)
{
    
    //データの設定
    setRanking(json["ranking"].string_value());
    
    if(_ranking == "1"){
        _ranking_image_type = RankingImageType::FIRST_PLACE ;
    }else if(_ranking == "2"){
        _ranking_image_type = RankingImageType::SECOND_PLACE ;
    }else{
        _ranking_image_type = RankingImageType::UNDER_THIRD_PLACE ;
    }
    
    int count = json["count"].int_value();
    for(int i=0; i<count; i++){
        string str_index = toString<int>(i) ;
        _reward_data.push_back(json[str_index]) ;
    }
}


void TournamentEventRewardSprite::createRewardImage()
{
    string item_type = _reward_data[0]["item_type"].string_value() ;
    string id = _reward_data[0]["related_primary_id"].string_value() ;
    
    //報酬画像
    string filename = "";
    if(item_type=="chara") filename = "download/Chara/all400/" + id + ".png";
    else if(item_type=="gold") filename = "init/Present/item2_400.png";
    else if(item_type=="jewel") filename = "init/Present/item_400.png";
    
    Sprite *reward = Sprite::create(filename);
    
    if(_ranking_image_type == RankingImageType::FIRST_PLACE){
        reward->setPosition(Point(117, 104));
        reward->setScale(0.38);
    } else if(_ranking_image_type == RankingImageType::SECOND_PLACE){
        reward->setPosition(Point(98, 70));
        reward->setScale(0.25);
    } else if(_ranking_image_type == RankingImageType::UNDER_THIRD_PLACE) {
        reward->setPosition(Point(98, 70));
        reward->setScale(0.25);
    }
    this->addChild(reward, 1);
}

void TournamentEventRewardSprite::createPlaceLabel()
{
    if(_ranking_image_type == RankingImageType::UNDER_THIRD_PLACE) {
        string place_text = "ベスト"+_ranking;
        Label *_end_date_label = HMSFontUtil::createWithTTFBySystemFontArgument(place_text, FONT_NAME_SYSTEM, 30);
        _end_date_label->setHorizontalAlignment(TextHAlignment::CENTER);
        _end_date_label->setPosition(Point(305, 127));
        this->addChild(_end_date_label) ;
    }
}

void TournamentEventRewardSprite::createRewardList()
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
    if(_ranking_image_type == RankingImageType::FIRST_PLACE) {
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
    if(_ranking_image_type == RankingImageType::FIRST_PLACE) {
        reward_label->setPosition(Point(229, 92));
    }else{
        reward_label->setPosition(Point(229, 60));
    }
    this->addChild(reward_label) ;
}

