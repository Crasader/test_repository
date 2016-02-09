//
//  FoxVersionPlugin.h
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import "AdManager.h"
class CCFoxPluginDelegate;

@interface FoxVersionPlugin : NSObject<CheckVersionDelegate>
{
    CCFoxPluginDelegate* foxVersionDelegate;
}

+ (FoxVersionPlugin*)sharedInstance;
- (void)checkVersionWithDelegate:(CCFoxPluginDelegate*) delegate;

@end
