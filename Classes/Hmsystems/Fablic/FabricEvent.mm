
#import <Foundation/Foundation.h>
#import "FabricEvent.h"
//#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>

void FabricEvent::Login(){
    [Answers logLoginWithMethod:@"digit" success:@YES customAttributes:@{}];//
}
