#ifndef __War__SupportInfoLayer__
#define __War__SupportInfoLayer__

#include "cocos2d.h"
#include "Chara.h"

USING_NS_CC;
using namespace std;

class SupportInfoLayer : public Layer
{
private:
    float ratio;
	
    HmsVector<string> _support_user_name_list;
    int _support_user_name_list_index = 0;
public:
    virtual bool init();
    CREATE_FUNC(SupportInfoLayer);
    
    void setBossEventSupportInfo(HmsVector<string> support_user_name_list);
    void updateBossEventSupportInfo(float dt);
};

#endif /* defined(__War__SupportInfoLayer__) */
