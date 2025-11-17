#pragma once

// native-lib.cpp
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "native_lib.hpp"

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceCreated(JNIEnv* env, jobject obj) {
        create();
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height) {
        update(width, height);
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeDrawFrame(JNIEnv* env, jobject obj) {
        draw();
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_initAssetManager(JNIEnv* env, jobject obj, jobject jAssetManager, jstring jInternalPath) {
        AAssetManager* assetMgr = AAssetManager_fromJava(env, jAssetManager);
        const char* internalPath = env->GetStringUTFChars(jInternalPath, nullptr);
        
        initAssetManager(assetMgr, internalPath);
        
        env->ReleaseStringUTFChars(jInternalPath, internalPath);
    }
}