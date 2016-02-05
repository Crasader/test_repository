#ifndef __war__DialogLayer__
#define __war__DialogLayer__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class DialogLayer : public Layer
{
public:
    virtual bool init();
    CREATE_FUNC(DialogLayer);
    
    void changeTurnDialog(string player_name, bool my_trun);
};
#endif /* defined(__war__DialogLayer__) */
