#include "InAppBillingManager.h"
#include "InAppBillingJni.h"

static InAppBillingDelegate *m_delegate;

void InAppBillingManager::setDelegate(InAppBillingDelegate* pDelegate)
{
	m_delegate = pDelegate;
}

void InAppBillingManager::initInAppBillingForIOS()
{
    //iOS用初期化処理
}

//購入要求
void InAppBillingManager::purchaseForItemId(std::string itemId, int userId)
{
    __android_log_print(10, "cocos jni test purchaseForItemId1", "purchaseForItemId1 %s", itemId.c_str());
    InAppBillingJni::purchaseForItemIdJni(itemId.c_str(), userId);
    __android_log_print(10, "cocos jni test purchaseForItemId2", "purchaseForItemId2 %s", itemId.c_str());
}

//成功時帰還
void InAppBillingManager::onSuccess(std::string pItemId)
{
    __android_log_print(10, "cocos jni test onSuccess_start", "onSuccess1 %s", "");
  if (m_delegate ){
    m_delegate->onSuccess(pItemId);
    __android_log_print(10, "cocos jni test onSuccess1", "onSuccess1 %s", pItemId.c_str());
  } else {
    __android_log_print(10, "cocos jni test onSuccess2", "onSuccess2 %s", pItemId.c_str());
  }
  __android_log_print(10, "cocos jni test onSuccess3", "onSuccess3 %s", pItemId.c_str());
}

//キャンセル時帰還
void InAppBillingManager::onCancel()
{
  if (m_delegate) {
    __android_log_print(10, "cocos jni test onCancel1", "onCancel1");
    m_delegate->onCancel();
  } else {
    __android_log_print(10, "cocos jni test onCancel2", "onCancel2");
  }
  __android_log_print(10, "cocos jni test onCancel3", "onCancel3");
}
