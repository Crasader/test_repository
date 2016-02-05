//
//  FoxVersionDelegate.h
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#ifndef __FoxVersionDelegate_h__
#define __FoxVersionDelegate_h__

class CCFoxPluginDelegate
{
public:
    virtual void didLoadVersion(const char* message) = 0;

};

namespace CCFoxPlugin {
    
    class CCFoxVersionPlugin
    {
    private:
        CCFoxPluginDelegate* foxVersionDelegate;
        
    public:
        CCFoxVersionPlugin();
        ~CCFoxVersionPlugin();
        
        static CCFoxVersionPlugin* sharedInstance();
        CCFoxPluginDelegate* getDelegate() {
            return foxVersionDelegate;
        }
        void checkVersionWithDelegate(CCFoxPluginDelegate* delegate);
    };
}

#endif /* defined(__FoxVersionDelegate_h__)*/
