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
        // 资源已在 initAssetManager() 中设置，这里不需要做任何事
        // 真正的初始化会在 nativeSurfaceChanged() 时进行（此时已获得正确的屏幕尺寸）
        // LOGI("Surface created, waiting for surface changed to initialize renderer");
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height) {
        // LOGI("Surface changed to %dx%d, initializing renderer", width, height);
        g_screenWidth = width;
        g_screenHeight = height;
        
        // 此时尺寸已确定，可以初始化渲染器
        rendererTestInit(width, height, g_resourcePrefix);
        rendererTestLogInfo();
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
