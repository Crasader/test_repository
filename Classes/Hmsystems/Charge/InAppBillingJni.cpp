#include "InAppBillingJni.h"
#include "common.h"
#include "HmsConsoleUtil.h"
#include <jni.h>
#include "InAppBillingManager.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

#define  LOG_TAG    "BrowserLauncher"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  CLASS_NAME "org/cocos2dx/lib/Cocos2dxActivity"


//呼び出すクラス名の指定
#if (RAKUTEN_FLG == 0)
#define  CLASS_NAME "org/cocos2dx/cpp/GooglePlayInAppBilling"
#elif (RAKUTEN_FLG == 1)
#define  CLASS_NAME "org/cocos2dx/rakutencpp/InAppBillingForRakuten"
#endif

//呼び出すメソッド名
static const char* const METHOD_NAME = "inAppBillingStart";

using namespace std;
using namespace cocos2d;


// c++ → Java 課金処理を行う
void InAppBillingJni::purchaseForItemIdJni(const char* itemId, int userId){
    
	JavaVM* jvm = JniHelper::getJavaVM();
	int status;
	JNIEnv *env;
	jmethodID mid;
	
	bool isAttached = false;
	
	
	// Get Status（JNIのバージョンチェック）
	status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
	if(status < 0)
	{
		// JNI インタフェースポインタ (JNIEnv) は、現在のスレッドでのみ有効です
		// 別のスレッドが Java VM にアクセスする必要がある場合、これは最初に AttachCurrentThread() を呼び出し、
		// それ自体を VM に接続し JNI インタフェースポインタを取得する必要があります。
		HMSLOG("callback_handler: failed to get JNI environment, assuming native thread...");
		status = jvm->AttachCurrentThread(&env, NULL);
		HMSLOG("Status 2: %d", status);
		if(status < 0)
		{
			HMSLOG("callback_handler: failed to attach current thread");
			return ;
		}
		isAttached = true;
		HMSLOG("Status isAttached: %d", isAttached);
	}
	//-----------------------------------------------------------
	
	HMSLOG("Status: %d", status);
	
	// Javaクラスを探す
	jclass mClass = env->FindClass(CLASS_NAME);
	
	
	// メソッドを探す
	mid = env->GetStaticMethodID(mClass, METHOD_NAME, "(Ljava/lang/String;I)V");
	
	// 引数をJavaのString型に変換する
	jstring stringArg = env->NewStringUTF(itemId);

	HMSLOG("mID: %d", mid);
	
	// メソッドを呼び出す
	if (mid!=0) {
		env->CallStaticVoidMethod(mClass, mid, stringArg, userId);
		HMSLOG("Call Method Finish");
	}
	else {
		HMSLOG("failed:Method could not be found... ClassName[%s] Method[%s]", CLASS_NAME, METHOD_NAME);
	}
	
	if(isAttached) {
		// デタッチする（後処理）
		jvm->DetachCurrentThread();
	}
	
	return ;
}



//java -> c++ 帰還用

//GooglePlay課金成功
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_GooglePlayInAppBilling_onSuccess(JNIEnv *env, jclass, jstring itemId) {
    //__android_log_print(10, "cocos jni test onSuccess", "onSuccess");
    const char* pszItemId = env->GetStringUTFChars(itemId, nullptr);
    InAppBillingManager::onSuccess(pszItemId);
    env->ReleaseStringUTFChars(itemId, pszItemId);
}

//GooglePlay課金失敗
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_GooglePlayInAppBilling_onCancel(JNIEnv *env, jclass) {
    //__android_log_print(10, "cocos jni test onCancel", "onCancel");
    InAppBillingManager::onCancel();
}

//楽天App市場課金成功
JNIEXPORT void JNICALL Java_org_cocos2dx_rakutencpp_InAppBillingForRakuten_onSuccess(JNIEnv *env, jclass, jstring itemId) {
    //__android_log_print(10, "cocos jni test onSuccess", "onSuccess");
    const char* pszItemId = env->GetStringUTFChars(itemId, nullptr);
    InAppBillingManager::onSuccess(pszItemId);
    env->ReleaseStringUTFChars(itemId, pszItemId);
}

//楽天App市場課金失敗
JNIEXPORT void JNICALL Java_org_cocos2dx_rakutencpp_InAppBillingForRakuten_onCancel(JNIEnv *env, jclass) {
    //__android_log_print(10, "cocos jni test onCancel", "onCancel");
    InAppBillingManager::onCancel();
}

