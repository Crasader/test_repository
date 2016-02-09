
#include "ComboEffectLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "SimpleAudioEngine.h"
#include "HmsUtilityFunction.h"
#include "Effect.h"
#include "PlistAnimationSprite.h"
#include "CharaList.h"

bool ComboEffectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    ratio = getVisibleRatio();
    return true;
}
void ComboEffectLayer::initialize(){
   
    
    
    
    
}
void ComboEffectLayer::combinationEffect(string combination_data,int index,int combiNum){
    
    HmsVector<string> value_data_list = split(combination_data,"@");//コンボキャラデータとvalueのデータに分ける
    
    HmsVector<string> combo_chara_pair = split(value_data_list.at(0),"&");//コンボキャラデータ分割
    for(Chara *chara : CharaList::getOneSide(index)){
        if(chara->isOnField()){
            if(to_string(chara->getPrimaryId())==combo_chara_pair.at(0)){
                chara_l=chara;
            }
            if(to_string(chara->getPrimaryId())==combo_chara_pair.at(1)){
                chara_r=chara;
            }
        }
    }
    
    //コンボ表示
    Point point_l = static_cast<DungeonScene *>(getParent())->fieldLayer->field_position_list[chara_l->getBelongingIndex()].at(chara_l->getFieldIndex());
    Point point_r = static_cast<DungeonScene *>(getParent())->fieldLayer->field_position_list[chara_r->getBelongingIndex()].at(chara_r->getFieldIndex());
    
    Point middle = (point_l + point_r)/2;
	
		//SuperAnimeアニメーション
//		std::string spritesheet_anim = FileUtils::getInstance()->fullPathForFilename("download/Effect/SAM/combo/combo.sam");
//		SuperAnim::SuperAnimNode* animNode1 = SuperAnim::SuperAnimNode::create(spritesheet_anim, 0, this);
//		this->addChild(animNode1);
//		animNode1->setTag(0);
//		animNode1->setPosition(Point(middle.x,middle.y + 100 * ratio));
//		animNode1->PlaySection("start");
//        animNode1->setScale(ratio);
	
    //スプライトシートアニメーション
    auto combo_sprite = PlistAnimationSprite::setData(this, "download/Effect/Sprite/combo", 1, 0.05f);
    combo_sprite->setPosition(middle.x+10*ratio,middle.y + 100 * ratio);
    combo_sprite->setScale(ratio);
    
    auto sprite2 = PlistAnimationSprite::setData(this, "download/Effect/Sprite/thunder_long", 1, 0.05f);
    sprite2->setPosition(middle);
    sprite2->setScale(ratio);
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo.mp3");
	//CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/katana.mp3");
    
    HmsVector<string> chara_prim_value_data = split(value_data_list.at(1),",");//キャラデータごと分割
    
    HmsVector<int> chara_primary;
    HmsVector<int> chara_new_value;
    
    for(int i=0;i<chara_prim_value_data.size();i++){
        HmsVector<string> temp =split(chara_prim_value_data.at(i),"&");
        chara_primary.push_back(stoi(temp.at(0)));
        chara_new_value.push_back(stoi(temp.at(1)));
    }
    
    
    for(int i=0;i<chara_primary.size();i++){
        if(chara_primary.at(i)==chara_l->getPrimaryId()){
            chara_l->showValue(chara_new_value.at(i),0.2f);//戦闘力の更新
        }else if(chara_primary.at(i)==chara_r->getPrimaryId()){
            chara_r->showValue(chara_new_value.at(i),0.2f);//戦闘力の更新
        }
    }
    
    if(combiNum==1){
        
        this->runAction(Sequence::create(DelayTime::create(0.5f),CallFunc::create([&](){//X秒後に演出終了
            static_cast<DungeonScene *>(getParent())->combinationCharacterEffectR(0.0f);//次へ
        }), nullptr));
        
    }else if(combiNum==2){
        
        this->runAction(Sequence::create(DelayTime::create(0.5f),CallFunc::create([&](){//X秒 後に演出終了
            static_cast<DungeonScene *>(getParent())->passiveSkillEffect(0.0f);//次へ
        }), nullptr));
    }else{
        HMSLOG("コンビネーションエフェクトエラー");
    }
}

void ComboEffectLayer::combinationThunder(int player_index){//ビリビリの表示
    
    vector<int> set_counter;
    int enemy_index = (player_index+1)%2;
    
    for(Chara *chara : CharaList::getAll()){
        if(chara->getFieldIndex()>1 && chara->isAlive() && !(chara->statusCheck(STATUS_TYPE_DARKNESS)==true && chara->getBelongingIndex() == enemy_index)){//フィールド内で
            int left_combo = chara->combination_count % 10;
            if(left_combo>0 ){//左側にコンボができている状況
                if(chara->getBelongingIndex() == player_index){//自分側
                    set_counter.push_back(chara->getFieldIndex()-2);
                }else{//敵側
                    set_counter.push_back(chara->getFieldIndex()+3);
                }
            }
        }
    }
    
    while(thunder.size()>set_counter.size()){//画像が多すぎる場合
        HMSLOG("画像多いので削除します");
        removeChild(thunder.back());//最終要素をリムーブ
        thunder.pop_back();//削除
    }
    while(thunder.size()<set_counter.size()){//画像が少なすぎる
        HMSLOG("画像が足りないので足します");
     
        auto temp = PlistAnimationSprite::setData(this, "download/Effect/Sprite/combo_thunder", 0, 0.07f);
        
        BlendFunc blend;//加算合成
        blend.src = GL_SRC_ALPHA;
        blend.dst = GL_ONE;
        temp->setBlendFunc(blend);
        
        thunder.insert(temp);
        //addChild(temp);
    }
    
    for(int i=0; i<set_counter.size(); i++){
        //CCLOG("セットカウンター：%d",set_counter.at(i));
        
        Point point;
        if(set_counter.at(i)<5){
            point = static_cast<DungeonScene *>(getParent())->fieldLayer->field_position_list[player_index].at(set_counter.at(i)+2);
        }else if(set_counter.at(i)<10){
            point = static_cast<DungeonScene *>(getParent())->fieldLayer->field_position_list[enemy_index].at(set_counter.at(i)-2);
        }else{
            CCLOG("おかしい");//set_counterに１０が入っていた
            break;
        }
        thunder.at(i)->setPosition(point + Vec2(50*ratio, 0));
        thunder.at(i)->setScale(ratio);
    }
    
    
    
    /*for(int k=0; k<set_counter.size(); k++){
        HMSLOG("セットカウンター%d",set_counter.at(k));
    }*/
}


