//
//  CARMediaCommon.h
//
//  Created by CAリワード on 13/11/18.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/** ユーザポイント管理 ドメイン */
#define UPM_BASE_URL            @"http://upm.mobadme.jp/upm/"

@interface CARMediaCommon : NSObject

// ユーザポイント管理開始処理
+ (void)startUserPointManager:(NSString*)publicAppKey
                      mediaId:(NSString*)mediaId
             mediaAccessToken:(NSString*)mediaAccessToken
                      userKey:(NSString*)userKey
                 userNickName:(NSString*)userNickName;
// ユーザポイント管理：ポイント交換
+ (void)spendUserPointManager:(NSString*)onetimeKey
                        trxId:(NSString*)trxId
                       scheme:(NSString*)scheme
                  trgtwebview:(UIWebView*)trgtwebview;

// メディア向け：uiid 準備
+ (BOOL) prepareUniqueInstallationIdentifier:(NSString*)keyString isDebug:(BOOL)isDebug;
// メディア向け：暗号化共通処理
+ (NSString *)ciphered:(NSString*)uiidString publicAppKey:(NSString*)publicAppKey;
// ハッシュ化
+ (NSString*) sha1:(NSString*)input;
+ (NSString *)string:(NSString *)string byURLEncoding:(NSStringEncoding)encoding;

@end
