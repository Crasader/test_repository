//
//  FoxVersionPlugin.m
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import "FoxVersionPlugin.h"
#import "Cocos2dxFox.h"
#import "FoxVersionDelegate.h"

@implementation FoxVersionPlugin

FoxVersionPlugin *sharedInstanceObj = nil;

+(FoxVersionPlugin*)sharedInstance {
    @synchronized(self) {
        if (!sharedInstanceObj) {
            sharedInstanceObj = [[FoxVersionPlugin alloc] init];
        }
    }
    return sharedInstanceObj;
}

-(void) checkVersionWithDelegate: (CCFoxPluginDelegate*)delegate {
    foxVersionDelegate = delegate;
    [[AppAdForceManager sharedManager] checkVersionWithDelegate:self];
}

-(void) didLoadVersion:(id)sender {
    BOOL matches = [[AppAdForceManager sharedManager] getBundleVersionStatus];
    const char *matchesNSStr = matches?"YES":"NO";
    foxVersionDelegate->didLoadVersion(matchesNSStr);
}
@end