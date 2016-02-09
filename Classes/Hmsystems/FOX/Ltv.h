//
//  Ltv.h
//  ForceOperationX v2.15.1g
//
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#define LTV_PARAM_SKU @"_sku"
#define LTV_PARAM_PRICE @"_price"
#define LTV_PARAM_CURRENCY @"_currency"
#define LTV_PARAM_OUT @"_out"

@interface AppAdForceLtv : NSObject {
@private
	id _ltv;
}

- (id)init;

// use all ltv
- (void)setLtvCookie;
- (void)ltvOpenBrowser:(NSString*)url;

- (void)sendLtv:(int)cvpointId;
- (void)sendLtv:(int)cvpointId :(NSString*)buid;

// change param  
- (void)addParameter:(NSString*)args :(NSString*)argv;
@end

// deprecated parameters
#define PARAM_SKU "_sku"
#define PARAM_PRICE "_price"
#define PARAM_OUT "_out"
