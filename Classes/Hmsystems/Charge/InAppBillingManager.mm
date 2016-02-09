#import "InAppBillingManager.h"
#import "InAppBilling_objc.h"

void InAppBillingManager::setDelegate(InAppBillingDelegate* pDelegate)
{
    [[InAppBilling sharedInAppBilling] addDelegate:pDelegate];
}

void InAppBillingManager::purchaseForItemId(std::string itemId, int userId)
{
    [[InAppBilling sharedInAppBilling] purchaseForItemId:[NSString stringWithUTF8String:itemId.c_str()] user_master_id:userId];
}

void InAppBillingManager::initInAppBillingForIOS()
{
    [[InAppBilling sharedInAppBilling] initInAppBillingForIOS];
}