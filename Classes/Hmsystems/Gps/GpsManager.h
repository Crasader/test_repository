#ifndef __GPS_MANAGER_H__
#define __GPS_MANAGER_H__

#include "cocos2d.h"

//GPS設定上の注意
//http://qiita.com/koogawa/items/6ec24ca3201977e9642b
//info.plistにNSLocationWhenInUseUsageDescriptionを追加

class GpsDelegate
{
public:
    virtual void onSuccess(std::string gps_data) {CC_UNUSED_PARAM(gps_data);}
    virtual void onCancel() {}
};

class GpsManager
{
public:
    static void setDelegate(GpsDelegate* pDelegate);
    static void requestLocation();
    
    static void onSuccess(std::string gps_data);
    static void onCancel();
};

#endif // __GPS_MANAGER_H__
