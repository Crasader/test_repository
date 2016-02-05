#import "InAppBilling_objc.h"

@implementation InAppBilling

@synthesize m_delegate;

static InAppBilling* _instance = nil;

#define BASE64PAD @"="

static const char   base64Alphabet[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

- (NSString *)stringEncodedWithBase64:(NSData *)data
{
    if (! data) return nil;
    
    NSUInteger      dataLen = data.length;
    unsigned char   *dataBytes = (unsigned char *)[data bytes];
    NSMutableString *str = [[NSMutableString alloc] init];
    
    NSUInteger  dataIndex = 0;
    while (dataIndex < dataLen) {
        char    d[3] = {0, 0, 0};
        d[0] = dataBytes[dataIndex];
        if ((dataIndex + 1) < dataLen)
            d[1] = dataBytes[dataIndex + 1];
        if ((dataIndex + 2) < dataLen)
            d[2] = dataBytes[dataIndex + 2];
        NSUInteger  bit6 = 0;
        char    s[5];
        
        bit6 = (d[0] >> 2) & 0x3F;
        s[0] = base64Alphabet[bit6];
        
        bit6 = ((d[1] >> 4) & 0x0F) | ((d[0] << 4) & 0x3F);
        s[1] = base64Alphabet[bit6];
        
        bit6 = ((d[2] >> 6) & 0x03) | ((d[1] << 2) & 0x3F);
        s[2] = base64Alphabet[bit6];
        
        bit6 = d[2] & 0x3F;
        s[3] = base64Alphabet[bit6];
        
        s[4] = '\0';
        
        [str appendString:[NSString stringWithCString:s encoding:NSASCIIStringEncoding]];
        
        dataIndex += 3;
    }
    if (dataIndex < dataLen) {
        NSRange aRange = NSMakeRange(dataLen + 2, (dataIndex - dataLen));
        [str replaceCharactersInRange:(NSRange)aRange withString:BASE64PAD];
    }
    
    NSUInteger  padNum = [str length] % 4;
    for (NSUInteger i = 0; i < padNum; i++) {
        [str appendString:BASE64PAD];
    }
    
    return str;
}

+ (id)sharedInAppBilling
{
    if (!_instance) {
		_instance = [[InAppBilling alloc] init];
	}
    return _instance;
}


+ (void) saveUserMaster: (int) userId
{
   NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
   [defaults setInteger:userId forKey:@"user_master_id"];
}

+ (int) loadUserMaster
{
   NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
   int userId = [defaults integerForKey:@"user_master_id"];
   return userId;
}

- (void) addDelegate: (InAppBillingDelegate *) delegate
{
    m_delegate = delegate;
}


//iOS用に起動時に初期化
- (void) initInAppBillingForIOS
{
    //キューに自身を登録
    if([SKPaymentQueue canMakePayments]){
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
}

- (void) purchaseForItemId:(NSString*)itemId user_master_id:(int)userId
{
    //購入者のuser_master_idをNSUserDefaultに保存(アプリ内に永続化)
    [InAppBilling saveUserMaster:userId];
    
    //購入処理
    if([SKPaymentQueue canMakePayments]){
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
        
        NSSet *productsIds = [NSSet setWithObject:itemId];
        SKProductsRequest *skProductsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productsIds];
        
        skProductsRequest.delegate = self;
        
        NSLog(@"ios purchase request start. product_id = %@, userId=%d", itemId, userId);
        [skProductsRequest start];
        
        /*
         IBOutlet UIWebView* webView;
         NSString* urlStr = @"http://sp.3cc-i.pkga.jp/connect.html";
         NSURL* url = [NSURL URLWithString: urlStr];
         NSURLRequest* urlReq = [ NSURLRequest requestWithURL: url];
         [webView loadRequest: urlReq];
         */
    } else {
       //[self showAlert:@"cannot purchase" text:@"設定 > 一般 > 機能制限で[App内での購入]をONにしてください"];
        UIAlertView * messageBox = [[UIAlertView alloc] initWithTitle: @"App内購入"
                                                              message: @"設定 > 一般 > 機能制限で[App内での購入]をONにしてください"
                                                             delegate: nil
                                                    cancelButtonTitle: @"OK"
                                                    otherButtonTitles: nil];
        [messageBox autorelease];
        [messageBox show];
        
        [self purchasingResult:NO];
    }
    NSLog(@"ios purchase request end");
    //[self showAlert:@"課金中"];
}

- (void) purchasedForItemId: (NSString*) itemId
{
    // 購入完了処理
    if (m_delegate)
    {
        m_delegate->onSuccess([itemId UTF8String]);
    }
}

- (void) canceled
{
    // 購入キャンセル処理
    if (m_delegate)
    {
        m_delegate->onCancel();
    }
}

#pragma mark -
#pragma mark SKProductsRequest Delegate

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"ios purchase request failed");
    
    //購入失敗
    [self purchasingResult:NO];

}

- (void)requestDidFinish:(SKRequest *)request
{
	[request release];
	NSLog(@"ios purchase request finished.");
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSLog(@"ios purchase didReceiveResponse");
    if (response == nil) {
        NSLog(@"ios purchase response is null");
        return;
    }
    
    bool validProduct = false;
    for(SKProduct *product in response.products) {
        SKPayment *payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
        validProduct = true;
    }
    
    //validProduct = true;
    NSLog(@"ios purchase product validation %d", validProduct);
    if(!validProduct) {
        [self purchasingResult:NO];
    }
}

#pragma mark -
#pragma mark SKPaymentTransactionObserver

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    
    /*
     購入処理の概要説明
     
     updatedTransactionsメソッドはなんらかの購入にかんする状態変化(遷移)が行われたときにStoreKitのシステムから呼ばれるメソッドである。
     (updatedTransactionsはSKPaymentTransactionObserverを継承し、オーバーライドしたメソッド)
     
     我々はupdatedTransactionsメソッドが呼ばれたとき、状態に応じて適した処理を実行すれば良い
     状態はtransaction.transactionStateの値を見れば確認することができる。
     transaction.transactionStateは以下の四つの値を取る
     
     1.SKPaymentTransactionStatePurchasing
     状態を反映するようにUIを更新し、再び呼び出されるのを待機。
     
     2.SKPaymentTransactionStateFailed
     errorプロパティの値を使用して、ユーザにメッセージを表示。
     
     3.SKPaymentTransactionStatePurchased
     購入した機能をユーザーに提供する。
     具体的には、user_masterテーブルのjewelを商品に応じた個数増やしたり、ログを保存したりしている。
     
     4.SKPaymentTransactionStateRestored
     以前に購入した機能を復元。
     この状態は、Non-consumable(非消耗型プロダクト)の復元のために使用する
     三国志バトルウォーズで使用しているプロダクトは、
     全てConsumable(消耗型プロダクト)なので、商品の復元は行わない、
     したがってこの値のチェックは行っていない。

     
     ・SKPaymentTransactionStatePurchased状態のときの処理の詳細
     transaction.transactionStateがSKPaymentTransactionStatePurchasedの値と同値の場合
     transaction.transactionReceiptには、購入されたプロダクトの情報(receipt)が格納されている。
     そのデータとuser_master_idを自社のサーバーにphpで伝えてjewelの反映やログの保存を行っている。
     
     ログの反映が行えたら [queue finishTransaction:transaction];
     を呼び出して課金処理の一連の流れ(トランザクション)が終了したことをStoreKitのシステムに伝える。
     (これを呼び出さなければStoreKitのシステムから、まだ購入した機能をユーザーに提供していないものと見なされ、
     何度もupdatedTransactionsメソッドが呼ばれてしまう)
     
     InAppPurchageの全ての情報は説明しきれないので、以下の公式の文書を参考にして下さい。
     参考:
     https://developer.apple.com/jp/documentation/StoreKitGuide.pdf
     
     */
    
    NSLog (@"transactions.count:%d", transactions.count) ;
    for(SKPaymentTransaction *transaction in transactions) {
        switch(transaction.transactionState) {
            case SKPaymentTransactionStatePurchasing:
                NSLog(@"Payment Now");
                //購入中
                break;
                
            case SKPaymentTransactionStatePurchased: {
                
                //user_master_idをNSUserDefaultから取り出し
                //購入リクエスト(purchaseForItemIdが呼ばれる)の際にNSUserDefaultにuser_master_idを保存しているので、
                //購入完了時(SKPaymentTransactionStatePurchased)にはuser_master_idが永続化済みであることが保証されている。
                int user_master_id = [InAppBilling loadUserMaster];
                
                NSLog(@"Payment OK");
                
                
                //サーバに送信(本番向け)
                NSLog(@"user_master_id:%d", user_master_id);
                NSString *idurlStr2 = [NSString stringWithFormat:@"%@%d", ITEM_SHOP_FINISH_PRO_URL, user_master_id];
                NSLog(@"idurlStr2:%@", idurlStr2);
                NSURL* idurl2 = [NSURL URLWithString: idurlStr2];
                NSMutableURLRequest *req2 = [NSMutableURLRequest requestWithURL:idurl2];
                [req2 setHTTPMethod:@"POST"];
                NSString *json2 = [NSString stringWithFormat:@"receipt-data=%@",
                                  [self stringEncodedWithBase64:transaction.transactionReceipt]];
                //NSLog(@"json:%@", json2);
                [req2 setHTTPBody:[json2 dataUsingEncoding:NSUTF8StringEncoding]];
                NSURLResponse *resp2;    // 結果を格納するオブジェクト
                NSError *err2;           // エラーを格納するオブジェクト
                NSData *result2 = [NSURLConnection sendSynchronousRequest:req2 returningResponse:&resp2 error:&err2];
                //NSString* resultStrPro = [[NSString alloc] initWithData:result2 encoding:NSUTF8StringEncoding];
                //NSLog(@"resultStrPro:%@", resultStrPro);

                
                //サーバに送信(開発向け)
                NSLog(@"user_master_id:%d", user_master_id);
                NSString *idurlStr = [NSString stringWithFormat:@"%@%d", ITEM_SHOP_FINISH_URL, user_master_id];
                NSLog(@"idurlStr:%@", idurlStr);
                NSURL* idurl = [NSURL URLWithString: idurlStr];
                NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:idurl];
                [req setHTTPMethod:@"POST"];
                NSString *json = [NSString stringWithFormat:@"receipt-data=%@",
                                  [self stringEncodedWithBase64:transaction.transactionReceipt]];
                //NSLog(@"json:%@", json);
                [req setHTTPBody:[json dataUsingEncoding:NSUTF8StringEncoding]];
                NSURLResponse *resp;    // 結果を格納するオブジェクト
                NSError *err;           // エラーを格納するオブジェクト
                NSData *result = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
                //NSString* resultStr = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
                //NSLog(@"resultStr:%@", resultStr);
                
                
                //通信で何らかのレスポンスがあれば成功
                if(result != nil || result2 != nil){
                    //購入成功
                    [queue finishTransaction:transaction];
                    [self purchasingResult:YES];
                }else{
                    [self purchasingResult:NO];
                }
                break;
            }
                
            case SKPaymentTransactionStateFailed:
                NSLog(@"Payment NG");
                NSLog(@"Error:%d", transaction.error.code);
                //購入失敗
                [queue finishTransaction:transaction];
                [self purchasingResult:NO];
                break;
        }
    }
}

- (void)purchasingResult:(BOOL)result
{
    NSLog(@"Payment result = %d", result);
    
    if(result == YES) {
        if(m_delegate != nil) {
            m_delegate->onSuccess("");
            NSLog(@"購入処理を成功しdelegateに成功の通知を行う");
        }else{
            NSLog(@"アプリ再開時、購入処理を完了");
        }
    }else{
        if(m_delegate != nil) {
            m_delegate->onCancel();
            NSLog(@"購入処理を失敗しdelegateに失敗の通知を行う");
        }else{
            NSLog(@"アプリ再開時、購入処理を失敗");
        }
    }
    
}

@end
