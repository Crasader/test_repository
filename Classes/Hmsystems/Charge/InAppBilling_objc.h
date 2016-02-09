#import "InAppBillingManager.h"
#import "common.h"
#import <StoreKit/StoreKit.h>

//#define ITEM_SHOP_FINISH_URL @"http://61.211.235.115/chrono/inapp_purchase.php?id="
#define ITEM_SHOP_FINISH_URL @"http://61.211.235.115/war_php/purchase/inapp_purchase.php?id="
#define ITEM_SHOP_FINISH_PRO_URL @"http://3bw.jp/purchase/inapp_purchase.php?id="

@interface InAppBilling : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    InAppBillingDelegate *m_delegate;
//    int userId;
}

@property(readwrite) InAppBillingDelegate *m_delegate;

- (NSString *)stringEncodedWithBase64:(NSData *)data;
+ (id) sharedInAppBilling;
+ (void) saveUserMaster:(int) userId;//user_master_idをアプリに保存する(NSUserDefaultを利用)
+ (int) loadUserMaster;//保存されているuser_master_idをアプリから取り出す(NSUserDefaultを利用)
- (void) initInAppBillingForIOS;
- (void) addDelegate: (InAppBillingDelegate *) delegate;
- (void) purchaseForItemId:(NSString*)itemId user_master_id:(int)userId;

- (void) purchasedForItemId: (NSString*) itemId;
- (void) canceled;

@end
