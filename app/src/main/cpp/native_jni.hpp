#pragma once

// native-lib.cpp
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
#include "native_lib.hpp"
#include "renderer_test.h"

// 用于存储屏幕尺寸
static int g_screenWidth = 0;
static int g_screenHeight = 0;
static std::string g_resourcePrefix = "";

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceCreated(JNIEnv* env, jobject obj) {
        // 使用 Renderer API 库进行渲染
        rendererTestInit(g_screenWidth, g_screenHeight, g_resourcePrefix);
        rendererTestLogInfo();
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height) {
        g_screenWidth = width;
        g_screenHeight = height;
        rendererTestInit(width, height, g_resourcePrefix);
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeDrawFrame(JNIEnv* env, jobject obj) {
        // 使用 Renderer API 库进行渲染
        rendererTestUpdate();
        rendererTestRender();
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_initAssetManager(JNIEnv* env, jobject obj, jobject jAssetManager, jstring jInternalPath) {
        AAssetManager* assetMgr = AAssetManager_fromJava(env, jAssetManager);
        const char* internalPath = env->GetStringUTFChars(jInternalPath, nullptr);
        
        // 先设置资源路径给 renderer_test
        rendererTestSetAssetManager(assetMgr, internalPath);
        
        // 更新本地的资源路径副本
        g_resourcePrefix = internalPath;
        
        env->ReleaseStringUTFChars(jInternalPath, internalPath);
    }
}
