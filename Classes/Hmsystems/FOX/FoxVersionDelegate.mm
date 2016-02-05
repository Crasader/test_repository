//
//  FoxVersionDelegate.m
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#include "FoxVersionDelegate.h"
#include "FoxVersionPlugin.h"


namespace CCFoxPlugin {
    static CCFoxVersionPlugin* sharedInstanceObject = nil;
    
    CCFoxVersionPlugin::CCFoxVersionPlugin()
    {
        foxVersionDelegate = NULL;
    }
    CCFoxVersionPlugin::~CCFoxVersionPlugin()
    {
        if (sharedInstanceObject)
        {
            delete sharedInstanceObject;
            sharedInstanceObject = NULL;
        }
    }
    
    CCFoxVersionPlugin* CCFoxVersionPlugin::sharedInstance()
    {
        if (! sharedInstanceObject)
        {
            sharedInstanceObject = new CCFoxVersionPlugin();
        }
        return sharedInstanceObject;
    }
    
    void CCFoxVersionPlugin::checkVersionWithDelegate(CCFoxPluginDelegate* delegate)
    {
        [[FoxVersionPlugin sharedInstance] checkVersionWithDelegate:delegate];
    }
    
}