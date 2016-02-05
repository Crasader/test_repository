#include <jni.h>

#ifndef __INAPP_BILLING_JNI_H__
#define __INAPP_BILLING_JNI_H__


class InAppBillingJni {
	
	public :
	static void purchaseForItemIdJni(const char* itemId, int userId);

};

extern "C" {
    
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_GooglePlayInAppBilling_onSuccess(JNIEnv* , jclass, jstring) ;
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_GooglePlayInAppBilling_onCancel(JNIEnv* , jclass) ;
    
    JNIEXPORT void JNICALL Java_org_cocos2dx_rakutencpp_InAppBillingForRakuten_onSuccess(JNIEnv* , jclass, jstring) ;
    JNIEXPORT void JNICALL Java_org_cocos2dx_rakutencpp_InAppBillingForRakuten_onCancel(JNIEnv* , jclass) ;
    
}

#endif // __INAPP_BILLING_JNI_H__
