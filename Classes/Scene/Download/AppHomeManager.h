#ifndef __APPHOMEMANAGER_H__
#define __APPHOMEMANAGER_H__

#include "cocos2d.h"
using namespace std;

class AppHomeManager
{
public:
    static string getAppHome();
    static bool iCloudTakeOff(std::string path);
};

#endif // __APPHOMEMANAGER_H__
