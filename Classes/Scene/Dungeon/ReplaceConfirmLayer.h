#ifndef __War__ReplaceConfirmLayer__
#define __War__ReplaceConfirmLayer__

#include "cocos2d.h"
#include "Chara.h"

USING_NS_CC;
using namespace std;

class ReplaceConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    Chara *replace_chara;
    Chara *tapped_chara;
public:
    float ratio;

    virtual bool init();
    CREATE_FUNC(ReplaceConfirmLayer);
	
    void setData(Chara *replace_chara, Chara *tapped_chara);
    void replace();
    void close();
	
	void popupAction();
	void deleteAction();
};

#endif /* defined(__War__ReplaceConfirmLayer__) */
