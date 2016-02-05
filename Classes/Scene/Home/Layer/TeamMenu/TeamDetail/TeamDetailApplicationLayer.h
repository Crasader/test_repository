#ifndef __war__TeamDetailApplicationLayer__
#define __war__TeamDetailApplicationLayer__

#include "TeamDetailBase.h"

USING_NS_CC;
using namespace std;

class TeamDetailApplicationLayer : public TeamDetailBase
{
private:
    
public:
    static TeamDetailApplicationLayer* create(json11::Json json);
    
    void createButton();
    void buttonTapAction();
    void buttonTapCallback(HttpClient *sender, HttpResponse *response);
};

#endif /* defined(__war__TeamDetailApplicationLayer__) */
