#include "TeamBattleCharacterStatus.h"

TeamBattleCharacterStatus* TeamBattleCharacterStatus::create(int hpMax, int level, float moveSpeed, int characterType, int characterId, TeamType teamType, std::string userName, std::string deckCharaIdList, std::string comment){
    TeamBattleCharacterStatus *node = new TeamBattleCharacterStatus();
    
    if (node && node->init(hpMax,level, moveSpeed, characterType, characterId,
                           teamType, userName, deckCharaIdList, comment)){
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool TeamBattleCharacterStatus::init(int hpMax, int level, float moveSpeed,
                                     int characterType,int characterId,
                                     TeamType teamType, std::string userName,
                                     std::string deckCharaIdList, std::string comment){
    if(!Node::init()) return false;

    _hp_max = hpMax;
    _hp = _hp_max;
    _level = level;
    _move_speed = moveSpeed;
    _character_type = characterType;
    _character_id = characterId;
    _team_type = teamType;
    _user_name = userName;
    _deck_chara_id_list = deckCharaIdList;
    _comment = comment;
    
    return true;
}