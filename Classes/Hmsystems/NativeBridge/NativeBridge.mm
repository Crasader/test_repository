//  NativeBridge.mm
//
//  Created by takataka5845 on 2014/02/24.
//
//

#import "NativeBridge.h"

using namespace cocos2d;

////////////////////// 指定URLをWebブラウザで開く //////////////////
bool NativeBridge::openURL(const char *pURL) {
	
	NSString *urlText = [NSString stringWithCString:pURL
										   encoding:NSUTF8StringEncoding];
	
	NSLog(@"input URL[%@]", urlText);
	
	BOOL result = YES;
	
	NSURL *url = [NSURL URLWithString:urlText];
	
	// 別スレッドで使える様に「__block修飾子」を付けてローカル変数を宣言する
	__block BOOL isOpneApp = false;
	
	// GCDで実行する
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),^{
		
		// 以下は別スレッドで処理される //////////////////////////////////////////
		NSLog(@"Start canOpenURL");
		// そのURLが使えるかチェック
		if ([[UIApplication sharedApplication] canOpenURL:url]) {
			
			NSLog(@"End canOpenURL is Done");
			NSLog(@"Start openURL");
			
			// 実際にそのURLを使う
			BOOL ret = [[UIApplication sharedApplication] openURL:url];
			
			// URLが使えなかった
			if (ret == NO) {
				NSLog(@"End openURL is Error");
				NSLog(@"URL Opne Error!! [%@]", url);
			}
			// URLが使えた
			else {
				NSLog(@"End openURL is Done");
				isOpneApp = true;
			}
		}
		// 使えなかった場合
		else {
			NSLog(@"End canOpenURL is Error");
			NSLog(@"Can not open the URL using the safari.app... URL[%@]", url);
		}
		/////////////////////////////////////////////////////////////////////
		
		// 以下はメインスレッドで処理される ///////////////////////////////////////
		dispatch_sync(dispatch_get_main_queue(), ^{
			
			//
			if (isOpneApp == false) {
				UIAlertView *alert = [[[UIAlertView alloc]initWithTitle:@"Error"
																message:@"「Mobile Safari」アプリが\n起動できませんでした"
															   delegate:nil
													  cancelButtonTitle:nil
													  otherButtonTitles:@"OK", nil]autorelease];
				[alert show];
			}
			
		});
		/////////////////////////////////////////////////////////////////////
		
	});
	
	// C++のbool型とObjective-CのBOOL型は変換が必要だよ!!
	if (result == YES) {
		return true;
	}
	return false;
}


////////////////////// lobi シリアルコードチェック //////////////////
bool NativeBridge::checkSerialRewardAndroid(const char *user_master_id, const char *password, bool is_pro){
    //アンドロイド用なので実装しない
    return false;
}