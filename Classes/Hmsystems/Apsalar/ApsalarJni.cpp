#include "ApsalarJni.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

#define LOG_TAG    "Apsalar"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define CLASS_NAME "com.apsalar.sdk.Apsalar"

typedef struct JniMethodInfo_
{
  JNIEnv *    env;
  jclass      classID;
  jmethodID   methodID;
} JniMethodInfo;

using namespace std;
extern "C"
{
  // get env and cache it                                                                      
  static JNIEnv* getJNIEnv(void)
  {
    JNIEnv *env = 0;

    // get jni environment                                                                   
    if (cocos2d::JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
      {
	LOGD("Failed to get the environment using GetEnv()");
      }

    if (cocos2d::JniHelper::getJavaVM()->AttachCurrentThread(&env, 0) < 0)
      {
	LOGD("Failed to get the environment using AttachCurrentThread()");
      }

    return env;
  }

  // get class and make it a global reference, release it at endJni().                         
  static jclass getClassID(JNIEnv *pEnv)
  {
    jclass ret = pEnv->FindClass(CLASS_NAME);
    if (! ret)
      {
	LOGD("Failed to find class of %s", CLASS_NAME);
      }

    return ret;
  }

  static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *methodName, const char *paramCode)
  {
    jmethodID methodID = 0;
    JNIEnv *pEnv = 0;
    bool bRet = false;

        do
	  {
            pEnv = getJNIEnv();
            if (! pEnv)
	      {
                break;
	      }

            jclass classID = getClassID(pEnv);

            methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
            if (! methodID)
	      {
                LOGD("Failed to find static method id of %s", methodName);
                break;
	      }

            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;

            bRet = true;
	  } while (0);

        return bRet;
  }

  void sendEventJni(const char* event_name)
  {

    //__android_log_print(10, "cocos test", "cocos %s,%d,%d", itemId,t.classID,t.methodID);                          

    JniMethodInfo methodInfo;

    if (! getStaticMethodInfo(methodInfo, "event", "(Ljava/lang/String;)V"))
      {
	return;
      }

    jstring stringArg = methodInfo.env->NewStringUTF(event_name);
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
    methodInfo.env->DeleteLocalRef(stringArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

    //__android_log_print(10, "cocos test2", "cocos2");                                                                    
  }
}
