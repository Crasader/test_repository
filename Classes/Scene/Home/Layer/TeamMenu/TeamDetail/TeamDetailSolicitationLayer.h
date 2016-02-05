#ifndef __war__TeamDetailSolicitationLayer__
#define __war__TeamDetailSolicitationLayer__

#include "TeamDetailBase.h"

USING_NS_CC;
using namespace std;

class TeamDetailSolicitationLayer : public TeamDetailBase
{
private:
    
public:
    static TeamDetailSolicitationLayer* create(json11::Json json);
    
    void createLayout();
    void createButton();
    void buttonTapAction();
    void buttonTapAction2();
    void buttonTapCallback(HttpClient *sender, HttpResponse *response);
    void buttonTapCallback2(HttpClient *sender, HttpResponse *response);
};

#endif /* defined(__war__TeamDetailSolicitationLayer__) */
