
#include "CharaList.h"
#include "TurnManage.h"

HmsVector<Chara *> CharaList::chara_list;


void CharaList::initialize(){
    
    chara_list.clear();
    
    
}

HmsVector<Chara *> CharaList::getAll(){
    return CharaList::chara_list;
}
HmsVector<Chara *> CharaList::getPlayer(){
    HmsVector<Chara *> temp;
    for(Chara *chara : CharaList::getAll()){
        if(chara->getBelongingIndex() == TurnManage::getPlayerIndex()){
            temp.insert(chara);
        }
    }
    return temp;
}
HmsVector<Chara *> CharaList::getEnemy(){
    HmsVector<Chara *> temp;
    for(Chara *chara : CharaList::getAll()){
        if(chara->getBelongingIndex() == TurnManage::getEnemyIndex()){
            temp.insert(chara);
        }
    }
    return temp;
}
HmsVector<Chara *> CharaList::getOneSide(int index){
    HmsVector<Chara *> temp;
    for(Chara *chara : CharaList::getAll()){
        if(chara->getBelongingIndex() == index){
            temp.insert(chara);
        }
    }
    return temp;
}
Chara* CharaList::getAtPrimary(int primary_id){
    /*for(Chara *chara : CharaList::getAll()){
        if(chara->getPrimaryId() == primary_id){
            return chara;
        }
    }*/
    
    auto itr = std::find_if(chara_list.begin(), chara_list.end(),[primary_id](Chara *chara){
        return (chara->getPrimaryId() == primary_id);
    });
    if(itr != chara_list.end()) return *itr;
    
    //CCLOG("ぬるぽ！");
    return nullptr;
}
Chara* CharaList::getAtField(int index, int field_index){
    if(field_index < 0) return nullptr;
    
    for(Chara *chara : CharaList::getOneSide(index)){
        if(chara->getFieldIndex() == field_index){
            return chara;
        }
    }
    //CCLOG("ぬるぽ！");
    return nullptr;
}

Chara* CharaList::getLeader(int index){
    for(Chara *chara : CharaList::getAll()){
        if(chara->getBelongingIndex() == index && chara->getType()==CHARA_TYPE_LEADER){
            return chara;
        }
    }
    //CCLOG("ぬるぽ！");
    return nullptr;
}
Chara* CharaList::getFriend(int index){
    for(Chara *chara : CharaList::getAll()){
        if(chara->getBelongingIndex() == index && chara->isFriend()){
            return chara;
        }
    }
    //CCLOG("ぬるぽ！");
    return nullptr;
}