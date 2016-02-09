#ifndef __war__TeamDetailSearchLayer__
#define __war__TeamDetailSearchLayer__

#include "TeamDetailBase.h"

USING_NS_CC;
using namespace std;

class TeamDetailSearchLayer : public TeamDetailBase
{
private:
    
public:
    static TeamDetailSearchLayer* create(json11::Json json);
    
    void createButton();
    void buttonTapAction();
    void buttonTapCallback(HttpClient *sender, HttpResponse *response);
};

#endif /* defined(__war__TeamDetailSearchLayer__) */
