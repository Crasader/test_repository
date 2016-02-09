#ifndef __War__SkillCheckLayer__
#define __War__SkillCheckLayer__

#include "cocos2d.h"
#include "Chara.h"
USING_NS_CC;
using namespace std;

class SkillCheckLayer : public Layer
{
private:
    float ratio;
	
public:
    virtual bool init();
    CREATE_FUNC(SkillCheckLayer);
    
    void setData(Chara *used_chara, int player_index);
	void skillstart();
    void close();
    vector<Sprite*> header_bg_img_list;
	Sprite* chara_select = nullptr;
	
    bool closing_flg = false;
};

#endif /* defined(__War__SkillCheckLayer__) */
