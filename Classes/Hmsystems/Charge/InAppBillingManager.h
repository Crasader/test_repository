#ifndef __INAPP_BILLING_H__
#define __INAPP_BILLING_H__

#include "cocos2d.h"

class InAppBillingDelegate
{
public:
    virtual void onSuccess(std::string pItemId) {CC_UNUSED_PARAM(pItemId);}
    virtual void onCancel() {}
};

class InAppBillingManager
{
public:
    static void initInAppBillingForIOS();
    static void setDelegate(InAppBillingDelegate* pDelegate);
    static void purchaseForItemId(std::string itemId, int userId);
    
    static void onSuccess(std::string itemId);
    static void onCancel();
};

#endif // __INAPP_BILLING_H__
