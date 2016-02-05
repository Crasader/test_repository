#include "GpsJni.h"
#include <jni.h>
#include "GpsManager.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

#define  LOG_TAG    "GpsJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  CLASS_NAME "org/cocos2dx/lib/Cocos2dxActivity"


//呼び出すクラス名の指定
#define  CLASS_NAME "jp/pkga/war/gps/Gps"

//呼び出すメソッド名
static const char* const METHOD_NAME = "GpsStart";

using namespace std;
using namespace cocos2d;


// c++ → Java 呼び出し処理を行う
void GpsJni::requestLocationJni(){
    
	JavaVM* jvm = JniHelper::getJavaVM();
	int status;
	JNIEnv *env;
	jmethodID mid;
	
	bool isAttached = false;
	
	
	// Get Status（JNIのバージョンチェック）
	status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
	LOGD("Status 1: %d", status);
	if(status < 0)
	{
		// JNI インタフェースポインタ (JNIEnv) は、現在のスレッドでのみ有効です
		// 別のスレッドが Java VM にアクセスする必要がある場合、これは最初に AttachCurrentThread() を呼び出し、
		// それ自体を VM に接続し JNI インタフェースポインタを取得する必要があります。
		LOGD("callback_handler: failed to get JNI environment, assuming native thread...");
		status = jvm->AttachCurrentThread(&env, NULL);
		LOGD("Status 2: %d", status);
		if(status < 0)
		{
		  LOGD("callback_handler: failed to attach current thread");
			return ;
		}
		isAttached = true;
		LOGD("Status isAttached: %d", isAttached);
	}
	//-----------------------------------------------------------
	
	LOGD("Status 3: %d", status);
	
	// Javaクラスを探す
	jclass mClass = env->FindClass(CLASS_NAME);
	
	
	// メソッドを探す
	mid = env->GetStaticMethodID(mClass, METHOD_NAME, "()V");
	
	// 引数をJavaのString型に変換する
	//jstring stringArg = env->NewStringUTF(itemId);

	LOGD("mID: %d", mid);
	
	// メソッドを呼び出す
	if (mid!=0) {
		env->CallStaticVoidMethod(mClass, mid);
		LOGD("Call Method Finish");
	}
	else {
	  LOGD("failed:Method could not be found... ClassName[%s] Method[%s]", CLASS_NAME, METHOD_NAME);
	}
	
	if(isAttached) {
		// デタッチする（後処理）
		jvm->DetachCurrentThread();
	}
	
	return ;
}



//java -> c++ 帰還用

JNIEXPORT void JNICALL Java_jp_pkga_war_gps_Gps_onSuccess(JNIEnv *env, jclass, jstring gps_data) {
    __android_log_print(10, "cocos jni test onSuccess", "onSuccess");
    const char* psz_gps_data = env->GetStringUTFChars(gps_data, nullptr);
    GpsManager::onSuccess(psz_gps_data);
    env->ReleaseStringUTFChars(gps_data, psz_gps_data);
}
