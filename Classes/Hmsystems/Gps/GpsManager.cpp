#include "GpsManager.h"
#include "GpsJni.h"

static GpsDelegate *m_delegate;

void GpsManager::setDelegate(GpsDelegate* pDelegate)
{
  m_delegate = pDelegate;
}

//購入要求
void GpsManager::requestLocation()
{
  __android_log_print(10, "cocos jni test requestLocation1", "requestLocation1");
  GpsJni::requestLocationJni();
  __android_log_print(10, "cocos jni test requestLocation2", "requestLocation2");
}

//成功時帰還
void GpsManager::onSuccess(std::string gps_data)
{
  __android_log_print(10, "cocos jni test onSuccess_start", "onSuccess1 %s", "");
  if (m_delegate ){
    m_delegate->onSuccess(gps_data);
    __android_log_print(10, "cocos jni test onSuccess1", "onSuccess1 %s", gps_data.c_str());
  } else {
    __android_log_print(10, "cocos jni test onSuccess2", "onSuccess2 %s", gps_data.c_str());
  }
  __android_log_print(10, "cocos jni test onSuccess3", "onSuccess3 %s", gps_data.c_str());
}
