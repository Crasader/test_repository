//  NativeBridge.cpp
//
//  Created by takataka5845 on 2014/02/24.
//
//

#include "NativeBridge.h"
#include "HmsConsoleUtil.h"
#include <android/log.h>
#include <jni.h>
//#include "cocoa/CCString.h"
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"

// Eclipse側のパッケージ名とjavaファイル名を指定
#if (RAKUTEN_FLG == 0)
#define kCLASS_NAME "org/cocos2dx/cpp/AppActivity"
#elif (RAKUTEN_FLG == 1)
#define kCLASS_NAME "org/cocos2dx/rakutencpp/AppActivityForRakuten"
#endif


using namespace std;
using namespace cocos2d;

////////////////////// 指定URLをWebブラウザで開く //////////////////
// 呼び出すJavaメソッド名
bool NativeBridge::openURL(const char *url)
{
    const char* METHOD_NAME = "onOpenURL" ;
	HMSLOG("start Static InterfaceJNI::openURL():[%s]", url);
    
	JavaVM* jvm = JniHelper::getJavaVM();
	int status;
	JNIEnv * env;
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
			return false;
		}
		isAttached = true;
		HMSLOG("Status isAttached: %d", isAttached);
	}
	//-----------------------------------------------------------
	
	HMSLOG("Status: %d", status);
	
	// Javaクラスを探す
	jclass mClass = env->FindClass(kCLASS_NAME);
	
	HMSLOG("jClass ");
	
	// メソッドを探す
	mid = env->GetStaticMethodID(mClass, METHOD_NAME, "(Ljava/lang/String;)V");
	
	// 引数をJavaのString型に変換する
	jstring stringArg = env->NewStringUTF(url);

	HMSLOG("mID: %d", mid);
	
	// メソッドを呼び出す
	if (mid!=0) {
		env->CallStaticVoidMethod(mClass, mid, stringArg);
		HMSLOG("Call Method Finish");
	}
	else {
		HMSLOG("failed:Method could not be found... ClassName[%s] Method[%s]", kCLASS_NAME, METHOD_NAME);
	}
	
	if(isAttached) {
		// デタッチする（後処理）
		jvm->DetachCurrentThread();
	}
 	
	return true;
}

////////////////////// lobi シリアルコードチェック //////////////////
bool NativeBridge::checkSerialRewardAndroid(const char *user_master_id, const char *password, bool is_pro)
{
    const char* METHOD_NAME = "checkSerialReward" ;
	HMSLOG("start Static InterfaceJNI::checkSerialReward():[user_maseter_id:%s password:%s, is_pro:%d]", user_master_id, password, is_pro);
	HMSLOG("この付近で落ちたらproj.android/src下を更新してください");
    
    cocos2d::JniMethodInfo minfo;
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(minfo, kCLASS_NAME, METHOD_NAME, "(Ljava/lang/String;Ljava/lang/String;Z)V");

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(user_master_id);
        jstring stringArg1 = minfo.env->NewStringUTF(password);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, (int)is_pro);
        
        minfo.env->DeleteLocalRef(stringArg0) ;
        minfo.env->DeleteLocalRef(stringArg1) ;
        minfo.env->DeleteLocalRef(minfo.classID) ;
    }
    
    return true ;
}
