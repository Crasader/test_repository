#ifndef __war__TeamBattleCharacterStatus__
#define __war__TeamBattleCharacterStatus__

#include "cocos2d.h"

USING_NS_CC;

class TeamBattleCharacterStatus : public Node{
public:
    enum TeamType{
        kFriend = 0,
        kEnemy,
    };
    
    static TeamBattleCharacterStatus* create(int hpMax, int level, float moveSpeed,
                                             int characterType, int characterId,
                                             TeamType teamType, std::string userName,
                                             std::string deckCharaIdList,
                                             std::string comment);
    
private:
    virtual bool init(int hpMax, int level, float moveSpeed,int characterType,
                      int characterId, TeamType teamType, std::string userName,
                      std::string deckCharaIdList, std::string comment);
    
    CC_SYNTHESIZE_READONLY(int, _hp, Hp);
    CC_SYNTHESIZE_READONLY(int, _hp_max, HpMax);
    CC_SYNTHESIZE_READONLY(int, _level, Level);
    CC_SYNTHESIZE_READONLY(float, _move_speed, MoveSpeed);
    CC_SYNTHESIZE_READONLY(int, _character_type, CharacterType);
    CC_SYNTHESIZE_READONLY(int, _character_id, CharacterId);
    CC_SYNTHESIZE_READONLY(TeamType, _team_type, TeamType);
    CC_SYNTHESIZE_READONLY(std::string, _user_name, UserName);
    CC_SYNTHESIZE_READONLY(std::string, _deck_chara_id_list, DeckCharaIdList);
    CC_SYNTHESIZE_READONLY(std::string, _comment, Comment);
    CC_SYNTHESIZE_READONLY(bool, _is_camp, IsCamp);
    CC_SYNTHESIZE_READONLY(bool, _isBattle, IsBattle);
};

#endif /* defined(__war__TeamBattleCharacterStatus__) */
