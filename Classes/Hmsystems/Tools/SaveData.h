//
//  SaveData.h
//
//
//  Created by albatrus on 2014/06/10.
//
//

#ifndef __GitTutorial__SaveData__
#define __GitTutorial__SaveData__

#include "cocos2d.h"

using namespace std;

class SaveData : public cocos2d::Node {
    
    
public:
    
    virtual  bool init();
    
    static void saveInt(const char *key,int value);
    static void saveFloat(const char *key,float value);
    static void saveString(const char *key,const string string);
    static void saveBool(const char *key,bool _bolean);
    
    static int loadInt(const char *key);
    static float loadFloat(const char *key);
    static string loadString(const char *key);
    static bool loadBool(const char *key);
    
};

//UserDefaultキー 一覧
#define UDK_USER_MASTER_ID "user_master_id"
#define UDK_PASSWORD "password"

#define UDK_USER_NAME "user_name"
#define UDK_USER_EXPERIENCE "user_experience"
#define UDK_LEVEL "level"
#define UDK_JEWEL "jewel"
#define UDK_GREETING_POINT "greeting_point"
#define UDK_GOLD "gold"
#define UDK_MAX_COST "max_cost"
#define UDK_POWER "power"
#define UDK_MAX_POWER "max_power"
#define UDK_FRIEND_ID "friend_id"

#define UDK_NOW_NEED_EXPERIENCE "now_need_experience"
#define UDK_NOW_NEED_TOTAL_EXPERIENCE "now_need_total_experience"
#define UDK_NEXT_NEED_EXPERIENCE "next_need_experience"
#define UDK_NEXT_NEED_TOTAL_EXPERIENCE "next_need_total_experience"

#define UDK_USER_TITLE "user_title"

#define UDK_USER_DATA_LIST "user_data_list"
#define UDK_USER_EXP_LIST "user_exp_list"
#define UDK_USER_CHARA_LIST "user_chara_list"
#define UDK_USER_TITLE_LIST "user_title_list"
#define UDK_USER_PRESENT_LIST "user_present_list"


#define UDK_RESOURCE_EFFECT_FLG "resource_effect_flg"
#define UDK_RESOURCE_VOICE_FLG "resource_voice_flg"

#define UDK_MY_SATGE_ID "my_satge_id"
#define UDK_STAGE_TSV_TEXT "stage_tsv_text"

#define UDK_DOWNLOAD_STEP "download_step"
#define UDK_RESOURCE_DUNGEON_ID "resource_dungeon_id"
#define UDK_RESOURCE_SPECIAL_DUNGEON_ID "resource_special_dungeon_id"

#define UDK_TUTORIAL_STATUS "tutorial_status"

#define UDK_BGM_VOLUME "bgm_volume" //BGM音量
#define UDK_EFFECT_VOLUME "effect_volume" //効果音音量

#define UDK_EFFECT "effect" //エフェクトのon off

#define UDK_GACHA_TAB "gacha_tab" //最後に開いたガチャタブ


#endif /* defined(__GitTutorial__SaveData__) */
