//
//  Cocos2dxFox.mm
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#include "Cocos2dxFox.h"
#import "AdManager.h"
#import "Ltv.h"
#import "AnalyticsManager.h"

static NSMutableDictionary* parms = nil;

void FoxPlugin::sendConversion(const char* url)
{
    NSString* str = [[NSString alloc]initWithCString:url encoding:NSUTF8StringEncoding];
    [[AppAdForceManager sharedManager]sendConversionWithStartPage:str];
}

void FoxPlugin::sendConversion(const char* url, const char* buid)
{
    NSString* str = [[NSString alloc]initWithCString:url encoding:NSUTF8StringEncoding];
    NSString* strid = [NSString stringWithCString: buid encoding:NSUTF8StringEncoding];
    [[AppAdForceManager sharedManager]sendConversionWithStartPage:str buid:strid];
}


void FoxPlugin::sendLtv(int ltvpoint)
{
    AppAdForceLtv* ltv =[[AppAdForceLtv alloc] init];
    for (id key in parms ) {
        [ltv addParameter:key :[parms objectForKey:key]];
    }
    [ltv sendLtv:ltvpoint];
    [ltv release];
    [parms removeAllObjects];
    [parms release];
    parms = nil;
}

void FoxPlugin::sendLtv(int ltvpoint, const char* ad)
{
    NSString* str = [NSString stringWithCString: ad encoding:NSUTF8StringEncoding];
    AppAdForceLtv* ltv =[[AppAdForceLtv alloc] init];
    for (id key in parms ) {
        [ltv addParameter:key :[parms objectForKey:key]];
    }
    [ltv sendLtv:ltvpoint :str];
    [ltv release];
    [parms removeAllObjects];
}

void FoxPlugin::addParameter(const char* name, const char* value)
{
    NSString* str = [NSString stringWithCString: name encoding:NSUTF8StringEncoding];
    NSString* val = [NSString stringWithCString: value encoding:NSUTF8StringEncoding];
    
    if (parms == nil) {
        parms = [[NSMutableDictionary alloc] init];
    }
    [parms setObject:val forKey:str];
}


void FoxPlugin::sendStartSession()
{
    [ForceAnalyticsManager sendStartSession];
}

void FoxPlugin::sendEvent(const char* eventName, const char* action, const char* label, int value)
{
    if(action == NULL) {
        action = "";
    }
    if(label == NULL) {
        label = "";
    }
    
    NSString* _eventName = [NSString stringWithCString: eventName encoding:NSUTF8StringEncoding];
    NSString* _action    = [NSString stringWithCString: action    encoding:NSUTF8StringEncoding];
    NSString* _label     = [NSString stringWithCString: label     encoding:NSUTF8StringEncoding];
    [ForceAnalyticsManager sendEvent:_eventName action:_action label:_label value:value];
}

void FoxPlugin::sendEvent(const char* eventName, const char* action, const char* label, const char* orderID, const char* sku, const char* itemName, double price, int quantity, const char* currency)
{
    if(action == NULL) {
        action = "";
    }
    if(label == NULL) {
        label = "";
    }
    if(orderID == NULL) {
        orderID = "";
    }
    if(sku == NULL) {
        sku = "";
    }
    if(itemName == NULL) {
        itemName = "";
    }
    if(currency == NULL) {
        currency = "";
    }
    
    NSString* _eventName = [NSString stringWithCString: eventName encoding:NSUTF8StringEncoding];
    NSString* _action    = [NSString stringWithCString: action    encoding:NSUTF8StringEncoding];
    NSString* _label     = [NSString stringWithCString: label     encoding:NSUTF8StringEncoding];
    NSString* _orderID   = [NSString stringWithCString: orderID   encoding:NSUTF8StringEncoding];
    NSString* _sku       = [NSString stringWithCString: sku       encoding:NSUTF8StringEncoding];
    NSString* _itemName  = [NSString stringWithCString: itemName  encoding:NSUTF8StringEncoding];
    NSString* _currency  = [NSString stringWithCString: currency  encoding:NSUTF8StringEncoding];
    
    [ForceAnalyticsManager sendEvent:_eventName action:_action label:_label orderID:_orderID sku:_sku itemName:_itemName price:price quantity:quantity currency:_currency];
}

void FoxPlugin::sendUserInfo(const char* userid, const char* attr1, const char* attr2, const char* attr3, const char* attr4, const char* attr5)
{
    if(attr1 == NULL) {
        attr1 = "";
    }
    if(attr2 == NULL) {
        attr2 = "";
    }
    if(attr3 == NULL) {
        attr3 = "";
    }
    if(attr4 == NULL) {
        attr4 = "";
    }
    if(attr5 == NULL) {
        attr5 = "";
    }
    
    NSString* _userid = [NSString stringWithCString: userid    encoding:NSUTF8StringEncoding];
    NSString* _attr1  = [NSString stringWithCString: attr1 encoding:NSUTF8StringEncoding];
    NSString* _attr2  = [NSString stringWithCString: attr2 encoding:NSUTF8StringEncoding];
    NSString* _attr3  = [NSString stringWithCString: attr3 encoding:NSUTF8StringEncoding];
    NSString* _attr4  = [NSString stringWithCString: attr4 encoding:NSUTF8StringEncoding];
    NSString* _attr5  = [NSString stringWithCString: attr5 encoding:NSUTF8StringEncoding];
    [ForceAnalyticsManager sendUserInfo:_userid attr1:_attr1 attr2:_attr2 attr3:_attr3 attr4:_attr4 attr5:_attr5];
}

void FoxPlugin::notifyManager()
{
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge
                                                                           | UIRemoteNotificationTypeSound
                                                                           | UIRemoteNotificationTypeAlert)];
}