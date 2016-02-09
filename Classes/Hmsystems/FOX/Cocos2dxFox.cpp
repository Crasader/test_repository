//
//  Cocos2dxFox.cpp
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#include <iostream>
#include "cocos2d.h"
#include "Cocos2dxFox.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>


static jobject getContext()
{
    cocos2d::JniMethodInfo t;
    cocos2d::JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;");
    jobject ctx = t.env->CallStaticObjectMethod(t.classID, t.methodID);
    return ctx;
}


void FoxPlugin::sendConversion(const char* urlString)
{
    

    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                            minfo,
                                                            "jp/appAdForce/android/cocos2dx/Cocos2dxAdManager",
                                                            "sendConversion",
                                                            "(Landroid/content/Context;Ljava/lang/String;)V"
                                                          );
    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(urlString);
                
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, stringArg0);
        minfo.env->DeleteLocalRef(stringArg0);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void FoxPlugin::sendConversion(const char* urlString, const char* buid)
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxAdManager",
                                                    "sendConversion",
                                                    "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V"
                                                    );

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(urlString);
        jstring stringArg1 = minfo.env->NewStringUTF(buid);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, stringArg0, stringArg1);
        minfo.env->DeleteLocalRef(stringArg0);
        minfo.env->DeleteLocalRef(stringArg1);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void FoxPlugin::sendLtv(int cvPoint)
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxLtvManager",
                                                    "sendLtvConversion",
                                                    "(Landroid/content/Context;I)V"
                                                    );

    if (exists) {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, (jint)cvPoint);
    }
}

void FoxPlugin::sendLtv(int cvPoint, const char* buid)
{
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxLtvManager",
                                                    "sendLtvConversion",
                                                    "(Landroid/content/Context;ILjava/lang/String;)V"
                                                    );

    if (exists) {
        jstring stringArg = minfo.env->NewStringUTF(buid);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, (jint)cvPoint, stringArg);
    }
}

void FoxPlugin::addParameter(const char* name, const char* value)
{
    
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxLtvManager",
                                                    "addParam",
                                                    "(Ljava/lang/String;Ljava/lang/String;)V"
                                                    );

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(name);
        jstring stringArg1 = minfo.env->NewStringUTF(value);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1);
    }
}


void FoxPlugin::sendStartSession()
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxAnalyticsManager",
                                                    "sendStartSession",
                                                    "(Landroid/content/Context;)V"
                                                    );

    if (exists) {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx);
    }
}

void FoxPlugin::sendEvent(const char* eventName, const char* action, const char* label, int value)
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxAnalyticsManager",
                                                    "sendEvent",
                                                    "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V"
                                                    );

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(eventName);
        jstring stringArg1 = minfo.env->NewStringUTF(action);
        jstring stringArg2 = minfo.env->NewStringUTF(label);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, stringArg0, stringArg1, stringArg2, value);
    }
}

void FoxPlugin::sendEvent(const char* eventName, const char* action, const char* label, const char* orderId, const char* sku, const char* itemName, double price, int quantity, const char* currency)
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxAnalyticsManager",
                                                    "sendEvent",
                                                    "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;DILjava/lang/String;)V"
                                                    );

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(eventName);
        jstring stringArg1 = minfo.env->NewStringUTF(action);
        jstring stringArg2 = minfo.env->NewStringUTF(label);
        jstring stringArg3 = minfo.env->NewStringUTF(orderId);
        jstring stringArg4 = minfo.env->NewStringUTF(sku);
        jstring stringArg5 = minfo.env->NewStringUTF(itemName);
        jstring stringArg6 = minfo.env->NewStringUTF(currency);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, stringArg0, stringArg1, stringArg2, stringArg3, stringArg4, stringArg5, price, quantity, stringArg6);
    }
}

void FoxPlugin::sendUserInfo(const char* userId, const char* userAttr1, const char* userAttr2, const char* userAttr3, const char* userAttr4, const char* userAttr5)
{
    
    jobject ctx = getContext();
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                    minfo,
                                                    "jp/appAdForce/android/cocos2dx/Cocos2dxAnalyticsManager",
                                                    "sendUserInfo",
                                                    "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"
                                                    );

    if (exists) {
        jstring stringArg0 = minfo.env->NewStringUTF(userId);
        jstring stringArg1 = minfo.env->NewStringUTF(userAttr1);
        jstring stringArg2 = minfo.env->NewStringUTF(userAttr2);
        jstring stringArg3 = minfo.env->NewStringUTF(userAttr3);
        jstring stringArg4 = minfo.env->NewStringUTF(userAttr4);
        jstring stringArg5 = minfo.env->NewStringUTF(userAttr5);
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, ctx, stringArg0, stringArg1, stringArg2, stringArg3, stringArg4, stringArg5);
    }
}

void FoxPlugin::notifyManager(const char* senderId)
{

    jobject ctx = getContext();
    cocos2d::JniMethodInfo miRegisterToGCM;
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                          miRegisterToGCM,
                                                          "jp/appAdForce/android/cocos2dx/Cocos2dxNotifyManager",
                                                          "registerToGCM",
                                                          "(Landroid/content/Context;Ljava/lang/String;)V"
                                                          );
    if (exists) {
        jstring stringArg = miRegisterToGCM.env->NewStringUTF(senderId);
        miRegisterToGCM.env->CallStaticVoidMethod(miRegisterToGCM.classID, miRegisterToGCM.methodID, ctx, stringArg);
    }

}

void FoxPlugin::updateFrom2_10_4g()
{
    cocos2d::JniMethodInfo minfo;
    
    bool exists = cocos2d::JniHelper::getStaticMethodInfo(
                                                          minfo,
                                                          "jp/appAdForce/android/cocos2dx/Cocos2dxAdManager",
                                                          "updateFrom2_10_4g",
                                                          "()V"
                                                          );
    if (exists) {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}