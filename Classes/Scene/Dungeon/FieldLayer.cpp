/*カードを置く場所に関係した処理*/

#include "FieldLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "Chara.h"
#include "TurnManage.h"
#include "CharaList.h"


#define DESIGN_FIELD_Z 102
#define DESIGN_FIELD_TAG 102 //102から100個使用

bool FieldLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    cardSize = Point(100,120);
    
    deck_point[0] = convertFromDisplaySize(Point(0,-500),0.5,0.5);//バトルの最初にカードが見えないように
    deck_point[1] = convertFromDisplaySize(Point(0,-500),0.5,0.5);
    
    
    for(int ii=0; ii<2; ii++) {
        if(ii==TurnManage::getPlayerIndex()) {
            castle_point[ii] = convertFromDisplaySize(Point(320, 264),0.5,0.5);
            friend_position_list[ii] = convertFromDisplaySize(Point(58,169),0.5,0.5);
            deck_point[ii] = convertFromDisplaySize(Point(650+cardSize.x/2,54),0.5,0.5);
            field_position_list[ii].push_back(convertFromDisplaySize(Point(195,264),0.5,0.5));
            field_position_list[ii].push_back(convertFromDisplaySize(Point(447,264),0.5,0.5));
            
            for(int i=0; i<6; i++){
                field_position_list[ii].push_back(convertFromDisplaySize(Point(70+100*i, 392),0.5,0.5));
            }
            
            for(int i=0; i<6; i++){
                hand_position_list[ii].push_back(convertFromDisplaySize(Point(59+i*104, 54),0.5,0.5));
            }
        } else {
            castle_point[ii] = convertFromDisplaySize(Point(320, 699),0.5,0.5);
            friend_position_list[ii] = convertFromDisplaySize(Point(58,790),0.5,0.5);
            deck_point[ii] = convertFromDisplaySize(Point(-10-cardSize.x/2, 906),0.5,0.5);
            field_position_list[ii].push_back(convertFromDisplaySize(Point(447,699),0.5,0.5));
            field_position_list[ii].push_back(convertFromDisplaySize(Point(195,699),0.5,0.5));
            
            for(int i=0; i<6; i++){
                field_position_list[ii].push_back(convertFromDisplaySize(Point(570-100*i, 576),0.5,0.5));
            }
            
            for(int i=0; i<6; i++){
                hand_position_list[ii].push_back(convertFromDisplaySize(Point(581-i*104, 906),0.5,0.5));
            }
        }
    }

    TTFConfig ttfConfig(FONT_NAME, 48, GlyphCollection::DYNAMIC);//ここから1.4倍の大きさに
    TTFConfig ttfConfigEffect(ttfConfig);
    
    
    cardSelectImage = Sprite::create("download/Chara/square/square_purple.png");
    cardSelectImage->setScale(ratio*0.315f);
    cardSelectImage->setVisible(false);
    
    addChild(cardSelectImage,3);
    
    return true;
}

//座標とタイプから自分のフィールドを判定する
int FieldLayer::getPositionIndex(Point tap_position, string field_type, int index){
    
    if(field_type == "field"){

        for(int i=0; i<field_position_list[index].size(); i++) {//field_podition_listの座標とカードの大きさから範囲を取得
            Rect rect = Rect(field_position_list[index].at(i).x - cardSize.x/2*ratio,
                             field_position_list[index].at(i).y - cardSize.y/2*ratio,
                             cardSize.x*ratio,
                             cardSize.y*ratio);
            if(rect.containsPoint(tap_position)) {
                return i;
            }
        }
        return -1;
        
    }else if(field_type =="hand"){
        
        for(int i=0; i<6; i++) {//field_podition_listの座標とカードの大きさから範囲を取得
            Rect rect = Rect(hand_position_list[index].at(i).x - cardSize.x/2*ratio,
                             hand_position_list[index].at(i).y - cardSize.y/2*ratio,
                             cardSize.x*ratio,
                             cardSize.y*ratio);
            if(rect.containsPoint(tap_position)) {
                return i;
            }
        }
        return -1;
        
    }else if(field_type == "leader"){
        
        Chara* target = CharaList::getLeader(index);
        Rect rect = Rect(target->getPosition().x - cardSize.x/2*ratio,
                         target->getPosition().y - cardSize.y/2*ratio,
                         cardSize.x*ratio,
                         cardSize.y*ratio);
        if(rect.containsPoint(tap_position)) {
            return 1;
        }
        return 0;
        
    }else if(field_type == "friend"){
        Rect rect = Rect(friend_position_list[index].x - cardSize.x/2*ratio,
                         friend_position_list[index].y - cardSize.y/2*ratio,
                         cardSize.x*ratio,
                         cardSize.y*ratio);
        if(rect.containsPoint(tap_position)) {
            return 1;
        }
        return 0;
    }else{
        HMSLOG("getPositionIndexエラー！！");
    }
    
    return -1;
}

bool FieldLayer::placeAbleField(int field_index, int index){//配置不可になっているフィールドか
    if(field_index<0 || field_index>7) return false;
    
    DungeonScene *dung = static_cast<DungeonScene*>(getParent());
    int rev_index = (index+1)%2;
    
    if(field_index == 0 || field_index == 1){
        for(int i=5; i<=7; i++){
            Chara *chara = getCharaFieldIndex(i - field_index*3, rev_index);
            if(chara!=nullptr && chara->statusCheck(STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT)){//目の前に前方軍師配置不可キャラがいる場合
                return false;
            }
        }
        
    }else if(field_index>=2 && field_index<=7){
        Chara *chara = getCharaFieldIndex(9-field_index, rev_index);
        if(chara!=nullptr && chara->statusCheck(STATUS_TYPE_PLACE_FORBIDDEN_FORWARD)){//置く場所の目の前に配置不可キャラがいる場合
            return false;
        }
    }
    
    for(int i=0; i<dung->player[index]->whole_status_active.size(); i++){//雪だるまがある場合
        if(dung->player[index]->whole_status_active.at(i)["state"]==STATUS_TYPE_SNOWMAN && dung->player[index]->whole_status_active.at(i)["value"]==field_index){
            return false;
        }
    }
    
    return true;
}


bool FieldLayer::equalPositionType(Chara *tapped_chara,int field_index ,int index){//キャラのタイプとフィールドのタイプが一致しているか（そこに置けるか）
    
    if(tapped_chara->isAttacker()||tapped_chara->isDefender()){
        if(field_index<2 || field_index>7) return false;
    }
    if(tapped_chara->getType() == CHARA_TYPE_SUPPORT && (field_index<0 || field_index>=2)){
        return false;
    }
    
    return placeAbleField(field_index, index);
}


void FieldLayer::fieldStateEffect(){
    DungeonScene *dung = static_cast<DungeonScene*>(getParent());
    
    vector<tuple<int, int, int, int>> vec_place_tup;//player index/field index/primary/置物No
    
    for(Chara *chara : CharaList::getAll()){
        if(chara->getFieldIndex()>1 && chara->statusCheck(STATUS_TYPE_PLACE_FORBIDDEN_FORWARD)){//前方武将配置不可スキル
            tuple<int, int, int, int> place_tup;
            get<0>(place_tup) = (chara->getBelongingIndex()+1)%2;
            get<1>(place_tup) = 9 - chara->getFieldIndex();
            get<2>(place_tup) = chara->getPrimaryId();
            get<3>(place_tup) = 1;//岩
            vec_place_tup.push_back(place_tup);
        }
        
        if(chara->getFieldIndex()>1 && chara->statusCheck(STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT)){//前方軍師配置不可スキル
            tuple<int, int, int, int> place_tup;
            get<0>(place_tup) = (chara->getBelongingIndex()+1)%2;
            if(chara->getFieldIndex()>=5){
                get<1>(place_tup) = 0;
            }else{
                get<1>(place_tup) = 1;
            }
            get<2>(place_tup) = chara->getPrimaryId();
            get<3>(place_tup) = 1;//岩
            vec_place_tup.push_back(place_tup);
        }
    }
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<dung->player[ii]->whole_status_active.size(); i++){//雪だるま封鎖
            if(dung->player[ii]->whole_status_active.at(i)["state"]==STATUS_TYPE_SNOWMAN){
                tuple<int, int, int, int> place_tup;
                get<0>(place_tup) = ii;
                get<1>(place_tup) = dung->player[ii]->whole_status_active.at(i)["value"];
                get<2>(place_tup) = dung->player[ii]->whole_status_active.at(i)["primary"];
                get<3>(place_tup) = 2;//雪だるま
                vec_place_tup.push_back(place_tup);
            }
        }
    }
    
    for(tuple<int, int, int, int> place_tup : vec_place_tup){
        Sprite *spr = getChildByName<Sprite *>(to_string(get<0>(place_tup))+"#"+to_string(get<1>(place_tup))+"#"+to_string(get<2>(place_tup)));
        if(spr == nullptr){//まだaddChildされていない時は追加する
            Sprite *temp_sprite;
            if(get<3>(place_tup) == 1){//岩の時
                temp_sprite = Sprite::create("download/Chara/icon/icon29.png");
                addChild(temp_sprite,100);
                temp_sprite->setOpacity(50);
                temp_sprite->setScale(ratio/3);
                temp_sprite->setPosition(field_position_list[get<0>(place_tup)].at(get<1>(place_tup))+Point(0,100*ratio));
                temp_sprite->runAction(Sequence::create(Spawn::create(EaseBackOut::create(ScaleTo::create(0.2f, ratio)),FadeTo::create(0.2f, 255), NULL),EaseExponentialIn::create(MoveTo::create(0.3f, field_position_list[get<0>(place_tup)].at(get<1>(place_tup)))), NULL) );
                
                runAction(Sequence::create(DelayTime::create(0.5f),CallFunc::create([&,place_tup](){
                    ParticleSystemQuad* ptcle = ParticleSystemQuad::create("download/Effect/Particle/card_set.plist");//煙エフェクト
                    ptcle->setAutoRemoveOnFinish(true);
                    ptcle->setScale(ratio);
                    ptcle->setPosition(field_position_list[get<0>(place_tup)].at(get<1>(place_tup))+Point(0,-50*ratio));
                    addChild(ptcle, 110);
                
                }), NULL));
                
            }else{//雪だるま
                temp_sprite = Sprite::create("download/Dungeon/yukidaruma.png");
                addChild(temp_sprite,101);
                temp_sprite->setPosition(field_position_list[get<0>(place_tup)].at(get<1>(place_tup)));
                temp_sprite->setScale(0.0f,ratio);
                temp_sprite->runAction(EaseBackOut::create(ScaleTo::create(0.3f, ratio)));
                
                /*runAction(Sequence::create(DelayTime::create(0.1f),CallFunc::create([&,place_tup](){
                    ParticleSystemQuad* ptcle = ParticleSystemQuad::create("download/Effect/Particle/card_set.plist");//煙エフェクト
                    ptcle->setAutoRemoveOnFinish(true);
                    ptcle->setScale(ratio);
                    ptcle->setPosition(field_position_list[get<0>(place_tup)].at(get<1>(place_tup))+Point(0,-50*ratio));
                    addChild(ptcle, 110);
                    
                }), NULL));*/
            }
            
            temp_sprite->setName(to_string(get<0>(place_tup))+"#"+to_string(get<1>(place_tup))+"#"+to_string(get<2>(place_tup)));
            temp_sprite->setTag(4989);
            
            
        }
    }
    
    for(auto child : getChildren()){
        Sprite *spr = dynamic_cast<Sprite *>(child);
        if(spr!=nullptr && spr->getTag() == 4989){//置物類のスプライトの中で
            HmsVector<string> place_hms = split(spr->getName(), "#");
            bool exist = false;
            for(tuple<int, int, int, int> place_tup : vec_place_tup){
                if(get<0>(place_tup) == stoi(place_hms.at(0)) && get<1>(place_tup) == stoi(place_hms.at(1)) && get<2>(place_tup) == stoi(place_hms.at(2))){//同じものがあったら
                    exist = true;
                    break;
                }
            }
            if(exist == false){//既にないものだったら消す
                spr->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.5f, ratio/2)),FadeTo::create(0.3f, 0), NULL),RemoveSelf::create(), NULL));
                //removeChild(spr);
            }
        }
    }
}

Chara* FieldLayer::getCharaFieldIndex(int field_index, int index){//フィールドインデックスからキャラを取得
    Chara *temp = nullptr;
    for(Chara *chara : CharaList::getOneSide(index)){
        if(chara->getFieldIndex() == field_index){
            temp = chara;
        }
    }
    return temp;
}

Point FieldLayer::getCharaPosition(Chara* chara){
    Point point = Point(0,0);
    if(chara->getFieldIndex() >= 0){
        point = field_position_list[chara->getBelongingIndex()].at(chara->getFieldIndex());
    }else if(chara->getHandIndex() >= 0){
        point = hand_position_list[chara->getBelongingIndex()].at(chara->getHandIndex());
    }else if(chara->getType() == CHARA_TYPE_LEADER){
        point = castle_point[chara->getBelongingIndex()];
    }
    
    return point;
}

bool FieldLayer::fieldBlankCheck(int field_index ,int index){//フィールドインデックスの場所が空白なのか
    if(field_index<0 || field_index>7) return false;
    for(Chara *chara : CharaList::getOneSide(index)){
        if(chara->getFieldIndex() == field_index){
            return false;
        }
    }
    return true;
}


